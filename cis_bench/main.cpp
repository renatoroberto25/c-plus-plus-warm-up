#include <poppler-document.h>
#include <poppler-page.h>
#include <nlohmann/json.hpp>
#include <xlsxwriter.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

struct Recommendation {
    std::string number, level, title;
    std::string profile, description, rationale, impact;
    std::string audit, remediation, defval, references, addinfo;
    std::string compliance = "To Review";
};

static const std::regex TITLE_RGX(R"(^(\d+(?:\.\d+)+)\s*(\(L\d+\))?\s*(.*))");

/// Remove "Page 123"
std::string sanitize(const std::string& s) {
    static const std::regex pg(R"(\bPage\s+\d+\b)", std::regex::icase);
    return std::regex_replace(s, pg, "");
}

/// Extrai texto do PDF a partir de uma página
std::string read_pdf(const std::string& path, int start) {
    auto doc = std::unique_ptr<poppler::document>(poppler::document::load_from_file(path));
    if (!doc) throw std::runtime_error("Erro abrindo PDF");
    if (start < 1 || start > doc->pages()) throw std::runtime_error("Página inválida");

    std::string out;
    for (int i = start - 1; i < doc->pages(); ++i) {
        auto pg = std::unique_ptr<poppler::page>(doc->create_page(i));
        if (pg) {
            out += pg->text().to_latin1();
            out.push_back('\n');
        }
    }
    return out;
}

/// Parser simplificado: pega só títulos
std::vector<Recommendation> parse_recs(const std::string& text) {
    std::vector<Recommendation> recs;
    std::istringstream iss(text);
    std::string line;
    Recommendation cur;
    while (std::getline(iss, line)) {
        line = sanitize(line);
        std::smatch m;
        if (std::regex_match(line, m, TITLE_RGX)) {
            if (!cur.number.empty()) recs.push_back(cur);
            cur = {};
            cur.number = m[1].str();
            cur.level  = m[2].matched ? m[2].str() : "";
            cur.title  = m[3].str();
        }
    }
    if (!cur.number.empty()) recs.push_back(cur);
    return recs;
}

/// Escreve CSV
void write_csv(const std::vector<Recommendation>& recs, const fs::path& out) {
    std::ofstream f(out);
    f << "Compliance|Number|Level|Title\n";
    for (auto& r : recs) {
        f << r.compliance << "|" << r.number << "|" << r.level << "|" << r.title << "\n";
    }
}

/// Escreve JSON
void write_json(const std::vector<Recommendation>& recs, const fs::path& out) {
    json j;
    for (auto& r : recs) {
        j.push_back({
            {"Compliance", r.compliance},
            {"Number", r.number},
            {"Level", r.level},
            {"Title", r.title}
        });
    }
    std::ofstream f(out);
    f << j.dump(2);
}

/// Escreve Excel
void write_xlsx(const std::vector<Recommendation>& recs, const fs::path& out) {
    lxw_workbook *wb = workbook_new(out.c_str());
    lxw_worksheet *ws = workbook_add_worksheet(wb, "Recommendations");
    const char* hdr[] = {"Compliance","Number","Level","Title"};
    for (int c=0;c<4;++c) worksheet_write_string(ws, 0, c, hdr[c], nullptr);
    int row=1;
    for (auto& r : recs) {
        worksheet_write_string(ws, row, 0, r.compliance.c_str(), nullptr);
        worksheet_write_string(ws, row, 1, r.number.c_str(), nullptr);
        worksheet_write_string(ws, row, 2, r.level.c_str(), nullptr);
        worksheet_write_string(ws, row, 3, r.title.c_str(), nullptr);
        row++;
    }
    workbook_close(wb);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        spdlog::info("Uso: {} <pdf> <csv|json|excel> <start_page>", argv[0]);
        return 1;
    }
    std::string in = argv[1];
    std::string fmt = argv[2];
    int start = std::stoi(argv[3]);

    auto text = read_pdf(in, start);
    auto recs = parse_recs(text);

    fs::path out = fs::path(in).replace_extension(fmt == "excel" ? ".xlsx" : "."+fmt);
    if (fmt=="csv") write_csv(recs, out);
    else if (fmt=="json") write_json(recs, out);
    else write_xlsx(recs, out);

    spdlog::info("Exportados {} itens para {}", recs.size(), out.string());
}