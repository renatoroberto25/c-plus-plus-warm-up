# 📑 CIS Bench Extractor

Ferramenta em **C++20** para extrair recomendações de benchmarks de segurança (ex. CIS Benchmarks) em **PDF** e exportar para **CSV**, **JSON** ou **Excel (.xlsx)**.

O objetivo é transformar documentos de benchmarks (normalmente pesados e manuais) em dados estruturados para análise, automação e integração com ferramentas de auditoria.

---

## 🚀 Funcionalidades
- Leitura de PDFs usando **poppler-cpp**  
- Extração de títulos numerados (ex.: `1.1.1.1 Ensure cramfs kernel module is not available`)  
- Sanitização de linhas (remove marcas como "Page 123")  
- Exportação em três formatos:
  - **CSV** (`.csv`)
  - **JSON** (`.json`)
  - **Excel** (`.xlsx` via libxlsxwriter)  
- Log amigável usando **spdlog**

---

## 🛠️ Dependências
- CMake ≥ 3.16
- Compilador com suporte a C++20
- Bibliotecas:
  - [poppler-cpp](https://poppler.freedesktop.org/)
  - [libxlsxwriter](https://libxlsxwriter.github.io/)
  - [spdlog](https://github.com/gabime/spdlog)
  - [nlohmann/json](https://github.com/nlohmann/json)

No Debian/Ubuntu, por exemplo:
```bash
sudo apt install g++ cmake pkg-config libpoppler-cpp-dev libxlsxwriter-dev

📦 Build
Clone o repositório e compile:
cmake -S . -B build
cmake --build build -j

O executável será gerado em build/cis_bench.

▶️ Uso
./build/cis_bench <arquivo.pdf> <csv|json|excel> <pagina_inicial>


📂 Estrutura do output
Compliance|Number|Level|Title
To Review|1.1.1.1|(L1)|Ensure cramfs kernel module is not available


📌 Roadmap futuro
Capturar também descrição, rationale, impacto, audit, remediation
Suporte a múltiplos benchmarks em lote
Interface web/CLI com filtros