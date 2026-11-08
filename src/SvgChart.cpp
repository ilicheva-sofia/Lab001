#include "SvgChart.hpp"

#include <algorithm>
#include <fstream>
#include <format>
#include <limits>
#include <sstream>

namespace Infrastructure {

    namespace {
        constexpr double margin_left = 80.0;
        constexpr double margin_right = 20.0;
        constexpr double margin_top = 50.0;
        constexpr double margin_bottom = 60.0;

        // Экранируем текст для XML
        std::string xml_escape(const std::string& s) {
            std::string out;
            out.reserve(s.size());
            for (char c : s) {
                switch (c) {
                    case '&': out += "&amp;"; break;
                    case '<': out += "&lt;"; break;
                    case '>': out += "&gt;"; break;
                    case '"': out += "&quot;"; break;
                    case '\'': out += "&apos;"; break;
                    default: out += c;
                }
            }
            return out;
        }

        double x_of(int point_index, double plot_width, int max_points) {
            return margin_left +
                   (plot_width * point_index) / (max_points - 1);
        }

        double y_of(double value, double plot_height, double min_value,
                    double max_value) {
            double clamped_value = std::clamp(value, min_value, max_value);
            return margin_top + plot_height -
                   ((clamped_value - min_value) / (max_value - min_value)) *
                       plot_height;
        }
    }  // namespace

    double SvgChart::min_of(const std::vector<Series>& series) {
        double mn = std::numeric_limits<double>::max();
        for (const auto& s : series)
            for (double v : s.values) mn = std::min(mn, v);
        return mn;
    }

    double SvgChart::max_of(const std::vector<Series>& series) {
        double mx = std::numeric_limits<double>::lowest();
        for (const auto& s : series)
            for (double v : s.values) mx = std::max(mx, v);
        return mx;
    }

    void SvgChart::write(const std::string& path, const std::string& title,
                         const std::vector<Series>& series, int width,
                         int height) {
        if (series.empty()) return;

        double mn = std::min(min_of(series), 0.0);  // баланс не уходит в минус
        double mx = max_of(series);
        if (mn == mx) mx += 1.0;
        // 5% отступа только сверху; снизу — жёсткий 0
        mx += (mx - mn) * 0.05;

        int max_points = 0;
        for (const auto& s : series)
            max_points = std::max(max_points, static_cast<int>(s.values.size()));
        if (max_points < 2) max_points = 2;

        double plot_w = width - margin_left - margin_right;
        double plot_h = height - margin_top - margin_bottom;

        std::ostringstream svg;
        svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
              "width=\"" << width << "\" height=\"" << height << "\" "
              "viewBox=\"0 0 " << width << " " << height << "\">\n";
        svg << "<style>text{font-family:sans-serif;font-size:12px;fill:#333;}"
              ".title{font-size:16px;font-weight:bold;}"
              ".legend{font-size:13px;}"
              ".axis{stroke:#999;stroke-width:1;}"
              ".grid{stroke:#e0e0e0;stroke-width:1;stroke-dasharray:4,4;}"
              ".line{fill:none;stroke-width:2;}</style>\n";

        // Фон
        svg << "<rect width=\"100%\" height=\"100%\" fill=\"#ffffff\"/>\n";

        // Заголовок
        svg << "<text x=\"" << width / 2.0 << "\" y=\"30\" text-anchor=\"middle\" "
              "class=\"title\">"
             << xml_escape(title) << "</text>\n";

        // Сетка и ось Y (6 линий, нижняя всегда 0)
        for (int i = 0; i <= 6; ++i) {
            double frac = i / 6.0;
            double v = mn + (mx - mn) * frac;
            double y = y_of(v, plot_h, mn, mx);
            svg << "<line class=\"grid\" x1=\"" << margin_left << "\" y1=\"" << y
                << "\" x2=\"" << width - margin_right << "\" y2=\"" << y << "\"/>\n";
            svg << "<text x=\"" << margin_left - 8 << "\" y=\"" << (y + 4)
                << "\" text-anchor=\"end\">" << std::format("{:.0f}", v)
                << "</text>\n";
        }

        // Оси
        svg << "<line class=\"axis\" x1=\"" << margin_left << "\" y1=\"" << margin_top
            << "\" x2=\"" << margin_left << "\" y2=\"" << height - margin_bottom
            << "\"/>\n";
        svg << "<line class=\"axis\" x1=\"" << margin_left << "\" y1=\""
            << height - margin_bottom << "\" x2=\"" << width - margin_right
            << "\" y2=\"" << height - margin_bottom << "\"/>\n";

        // Подписи оси X (каждый N-й)
        int label_step = std::max(1, (max_points - 1) / 10);
        for (int i = 0; i < max_points; i += label_step) {
            double x = x_of(i, plot_w, max_points);
            svg << "<text x=\"" << x << "\" y=\"" << height - margin_bottom + 18
                << "\" text-anchor=\"middle\">" << (i + 1) << "</text>\n";
        }
        // подпись последней точки всегда
        if ((max_points - 1) % label_step != 0) {
            svg << "<text x=\"" << x_of(max_points - 1, plot_w, max_points)
                << "\" y=\""
                << height - margin_bottom + 18 << "\" text-anchor=\"middle\">"
                << max_points << "</text>\n";
        }

        // Подпись осей
        svg << "<text x=\"" << width / 2.0 << "\" y=\"" << height - 8
            << "\" text-anchor=\"middle\">Раунд</text>\n";
        svg << "<text x=\"15\" y=\"" << height / 2.0
            << "\" text-anchor=\"middle\" transform=\"rotate(-90, 15, "
            << height / 2.0 << ")\">Баланс (RUB)</text>\n";

        // Линии данных
        for (const auto& s : series) {
            if (s.values.empty()) continue;
            std::ostringstream points;
            for (size_t i = 0; i < s.values.size(); ++i) {
                if (i) points << " ";
                points << x_of(static_cast<int>(i), plot_w, max_points) << ","
                       << y_of(s.values[i], plot_h, mn, mx);
            }
            svg << "<polyline class=\"line\" stroke=\"" << s.color
                << "\" points=\"" << points.str() << "\"/>\n";
        }

        // Легенда
        double legend_x = margin_left + 10;
        double legend_y = margin_top + 15;
        for (const auto& s : series) {
            svg << "<rect x=\"" << legend_x << "\" y=\"" << (legend_y - 10)
                << "\" width=\"16\" height=\"4\" fill=\"" << s.color << "\"/>\n";
            svg << "<text x=\"" << (legend_x + 22) << "\" y=\"" << legend_y
                << "\" class=\"legend\">"
                << xml_escape(s.name) << "</text>\n";
            legend_y += 20;
        }

        svg << "</svg>\n";

        std::ofstream out(path);
        out << svg.str();
    }

}  // namespace Infrastructure
