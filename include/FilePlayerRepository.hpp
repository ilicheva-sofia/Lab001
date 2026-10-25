#include "PlayerRepository.hpp"

namespace Repository {

    class FilePlayerRepository : public PlayerRepository {
        // реализация сохранения в файл
       private:
        std::string filename;

       public:
        FilePlayerRepository(std::string filename);
        std::shared_ptr<Domain::Player> load(Values::PlayerId id) override;
        void save(std::shared_ptr<Domain::Player> player) override;
    };
}  // namespace Repository