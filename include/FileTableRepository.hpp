#include "TableRepository.hpp"

namespace Repository {

    class FileTableRepository : public TableRepository {
        // реализация сохранения в файл
       private:
        std::string filename;

       public:
        FileTableRepository(std::string filename);
        std::shared_ptr<Aggregates::Table> load(Values::TableId id) override;
        void save(std::shared_ptr<Aggregates::Table> table) override;
    };
}  // namespace Repository