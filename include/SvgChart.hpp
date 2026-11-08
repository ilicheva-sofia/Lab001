#pragma once
#include <string>
#include <vector>

namespace Infrastructure {

    struct Series {
        std::string name;
        std::string color;  // hex,  "#e41a1c"
        std::vector<double> values;
    };

    class SvgChart {
       public:
        // width/height — в пикселях, margin — отступ для осей
        static void write(const std::string& path, const std::string& title,
                          const std::vector<Series>& series,
                          int width = 900, int height = 500);

        // Вспомогательный: вычисляет мин/макс по всем сериям
        static double min_of(const std::vector<Series>& series);
        static double max_of(const std::vector<Series>& series);
    };

}  // namespace Infrastructure
