#include <poppler-document.h>
#include <poppler-page.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <xlsxwriter.h>

#include <regex>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <optional>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct Rec {
  std::string number, level, title;
  std::string profileApplicability, description, rationale, impact,
              audit, remediation, defaultValue, references, additionalInfo;
  std::string complianceStatus = "To Review";
};

static const std::regex TITLE_RGX(R"(^(\d+(?:\.\d+)+)\s*(\(L\d+\))?\s*(.*))");
static const std::vector<std::string> SECTIONS = {
  "Profile Applicability:", "Description:", "Rationale:", "Impact:",
  "Audit:", "Remediation:", "Default Value:", "References:", "Additional Information:"
};

std::string rm_page_markers(const std::string &s) {
  static const std::regex pg(R"(\bPage\s+\d+\b)", std::regex::icase);
  return std::regex_replace(s, pg, "");
}

std::pair<std::string,std::string> extract_title_version(const std::string& pdfPath) {
  auto doc = std::unique_ptr<poppler::document>(poppler::document::load_from_file(pdfPath));
  if (!doc) throw std::runtime_error("Cannot open PDF");
  auto pg = std::unique_ptr<poppler::page>(doc->create_page(0));
  std::string title, version;
  if (pg) {
    auto txt = pg->text().to_latin1(); // ajuste: use to_utf8 se preferir
    std::istringstream iss(txt);
    std::string line;
    while (std::getline(iss, line)) {
      auto l = line;
      if (!l.empty() && (l[0]=='v' || l[0]=='V') && l.find('-') != std::string::npos) { version = l; break; }
      title += (title.empty()? "" : " ") + l;
    }
  }
  if (title.empty()) title = "CIS Benchmark Document";
  return {title, version};
}

std::string read_pdf_text(const std::string& pdfPath, int startPage) {
  auto doc = std::unique_ptr<poppler::document>(poppler::document::load_from_file(pdfPath));
  if (!doc) throw std::runtime_error("Cannot open PDF");
  if (startPage < 1 || startPage > doc->pages()) throw std::runtime_error("start_page out of range");
  std::string out;
  for (int i = startPage - 1; i < doc->pages(); ++i) {
    auto pg = std::unique_ptr<poppler::page>(doc->create_page(i));
    if (!pg) continue;
    auto t = pg->text().to_latin1();
    out += t;
    if (out.back() != '\n') out += "\n";
  }
  return out;
}

bool looks_like_rec_start(const std::vector<std::string>& lines, size_t idx) {
  // Procura "Profile Applicability:" em até ~10 linhas
  for (size_t i = idx + 1; i < std::min(lines.size(), idx + 11); ++i) {
    if (lines[i].rfind("Profile Applicability:", 0) == 0) return true;
    if (std::regex_match(lines[i], TITLE_RGX)) return false;
    for (auto &sec : SECTIONS) if (lines[i].rfind(sec, 0) == 0) return false;
  }
  return false;
}

std::pair<std::string,size_t> extract_section(const std::vector<std::string>& lines, size_t idx) {
  std::string acc;
  size_t cur = idx + 1;
  auto is_new_boundary = [&](const std::string& s){
    if (std::regex_match(s, TITLE_RGX)) return true;
    for (auto &sec : SECTIONS) if (s.rfind(sec, 0) == 0) return true;
    auto low = s; std::transform(low.begin(), low.end(), low.begin(), ::tolower);
    return low.rfind("cis controls", 0) == 0;
  };
  while (cur < lines.size()) {
    auto s = rm_page_markers(lines[cur]);
    if (is_new_boundary(s)) break;
    if (!acc.empty()) acc += " ";
    acc += s;
    ++cur;
  }
  return {acc, cur};
}

std::vector<Rec> parse_recs(const std::string& text) {
  std::vector<Rec> recs;
  std::vector<std::string> lines;
  {
    std::istringstream iss(text);
    std::string l;
    while (std::getline(iss, l)) lines.push_back(l);
  }
  std::optional<Rec> cur;
  for (size_t i=0;i<lines.size();++i) {
    auto s = rm_page_markers(lines[i]);
    std::smatch m;
    if (std::regex_match(s, m, TITLE_RGX)) {
      if (looks_like_rec_start(lines, i)) {
        if (cur) recs.push_back(*cur);
        cur = Rec{m[1].str(), m[2].matched ? m[2].str() : "", m[3].str()};
        // título multilinha
        while (i+1 < lines.size()) {
          auto nxt = lines[i+1];
          bool isSec = false;
          if (std::regex_match(nxt, TITLE_RGX)) break;
          for (auto &sec : SECTIONS) if (nxt.rfind(sec, 0)==0) { isSec=true; break; }
          if (isSec) break;
          ++i; cur->title += " " + lines[i];
        }
      }
    }
    if (!cur) continue;
    for (auto &sec : SECTIONS) {
      if (s.rfind(sec, 0) == 0) {
        auto [content, next] = extract_section(lines, i);
        auto key = sec.substr(0, sec.size()-1); // remove ':'
        if (key == "Profile Applicability") cur->profileApplicability = content;
        else if (key == "Description") cur->description = content;
        else if (key == "Rationale") cur->rationale = content;
        else if (key == "Impact") cur->impact = content;
        else if (key == "Audit") cur->audit = content;
        else if (key == "Remediation") cur->remediation = content;
        else if (key == "Default Value") cur->defaultValue = content;
        else if (key == "References") cur->references = content;
        else if (key == "Additional Information") cur->additionalInfo = content;
        i = next - 1;
        break;
      }
    }
  }
  if (cur) recs.push_back(*cur);
  // de-dup simples por (number,title)
  std::vector<Rec> uniq;
  std::set<std::pair<std::string,std::string>> seen;
  for (auto &r : recs) {
    auto k = std::make_pair(r.number, r.title);
    if (!seen.count(k)) { seen.insert(k); uniq.push_back(r); }
  }
  return uniq;
}

void write_csv(const std::vector<Rec>& recs, const fs::path& out, const std::string& title, const std::string& version) {
  std::ofstream f(out);
  f << title << "\n" << version << "\n\n";
  f << "Compliance Status|Number|Level|Title|Profile Applicability|Description|Rationale|Impact|Audit|Remediation|Default Value|References|Additional Information\n";
  auto esc = [](std::string s){ return s; }; // simples; separador é '|'
  for (auto &r : recs) {
    f << esc(r.complianceStatus) << '|'
      << esc(r.number) << '|' << esc(r.level) << '|' << esc(r.title) << '|'
      << esc(r.profileApplicability) << '|' << esc(r.description) << '|' << esc(r.rationale) << '|'
      << esc(r.impact) << '|' << esc(r.audit) << '|' << esc(r.remediation) << '|'
      << esc(r.defaultValue) << '|' << esc(r.references) << '|' << esc(r.additionalInfo) << "\n";
  }
}

void write_json(const std::vector<Rec>& recs, const fs::path& out, const std::string& title, const std::string& version) {
  json j;
  j["document_title"] = title;
  j["document_version"] = version;
  j["recommendations"] = json::array();
  for (auto &r : recs) {
    j["recommendations"].push_back({
      {"Compliance Status", r.complianceStatus},
      {"Number", r.number},
      {"Level", r.level},
      {"Title", r.title},
      {"Profile Applicability", r.profileApplicability},
      {"Description", r.description},
      {"Rationale", r.rationale},
      {"Impact", r.impact},
      {"Audit", r.audit},
      {"Remediation", r.remediation},
      {"Default Value", r.defaultValue},
      {"References", r.references},
      {"Additional Information", r.additionalInfo}
    });
  }
  std::ofstream f(out);
  f << j.dump(2);
}

void write_xlsx(const std::vector<Rec>& recs, const fs::path& out, const std::string& title, const std::string& version) {
  lxw_workbook *wb = workbook_new(out.c_str());
  lxw_worksheet *ws = workbook_add_worksheet(wb, "Recommendations");

  // Título e versão
  worksheet_write_string(ws, 0, 0, title.c_str(), nullptr);
  worksheet_write_string(ws, 1, 0, version.c_str(), nullptr);

  // Cabeçalho na linha 3 (index 3 = row 3 zero-based 2)
  const char* hdr[] = {"Compliance Status","Number","Level","Title","Profile Applicability","Description","Rationale","Impact","Audit","Remediation","Default Value","References","Additional Information"};
  for (int c=0;c<13;++c) worksheet_write_string(ws, 3, c, hdr[c], nullptr);

  int row = 4;
  for (auto &r : recs) {
    int c=0;
    worksheet_write_string(ws, row, c++, r.complianceStatus.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.number.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.level.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.title.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.profileApplicability.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.description.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.rationale.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.impact.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.audit.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.remediation.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.defaultValue.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.references.c_str(), nullptr);
    worksheet_write_string(ws, row, c++, r.additionalInfo.c_str(), nullptr);
    ++row;
  }
  workbook_close(wb);
}

int main(int argc, char** argv) {
  // CLI bem simples: cis_bench <input.pdf> <format: csv|json|excel> <start_page>
  if (argc < 2) {
    fmt::print("Usage: {} <input.pdf> [csv|json|excel] [start_page]\n", argv[0]);
    return 1;
  }
  std::string input = argv[1];
  std::string fmtOut = (argc>=3)? argv[2] : "excel";
  int start = (argc>=4)? std::stoi(argv[3]) : 10;

  try {
    auto [title, version] = extract_title_version(input);
    auto text = read_pdf_text(input, start);
    auto recs = parse_recs(text);

    fs::path base = fs::path(input).replace_extension();
    if (fmtOut == "csv") {
      write_csv(recs, base.concat(".csv"), title, version);
    } else if (fmtOut == "json") {
      write_json(recs, base.concat(".json"), title, version);
    } else {
      write_xlsx(recs, base.concat(".xlsx"), title, version);
    }
    spdlog::info("OK: {} recomendações processadas", recs.size());
  } catch (const std::exception& e) {
    spdlog::error("Erro: {}", e.what());
    return 2;
  }
  return 0;
}