#include "Money.hpp"

namespace entities {
    using namespace Values;


    class RequestLine {
      PlayerId id;
      Money balance;

    public:
      RequestLine(PlayerId id, Money Money)
        : id(id), Money(std::move(Money)) {
      }

      PlayerId get_id() const { return id; }
      const Money &get_Money() const { return Money; }
      Money calculate_amount() const { return Money.get_price() * quantity; }
    };


    class PurchaseRequest {
      RequestId id;
      AuthorId author;
      RequestStatus status = RequestStatus::draft;
      std::vector<RequestLine> lines;
      static constexpr Money limit = 100'000.0_RUB;

      void require_draft() const {
        if (status != RequestStatus::draft)
          throw errors::RequestNotDraft(id.value, static_cast<int>(status));
      }

      void ensure_addition_fits(Money current,
                                Money additional) const {
        if (additional > limit - current)
          throw errors::RequestLimitExceeded(
            id.value,
            current.get_kopecks(),
            additional.get_kopecks(),
            limit.get_kopecks()
          );
      }

    public:
      PurchaseRequest(RequestId id, AuthorId author) : id(id), author(author) {
      }

      RequestId get_id() const { return id; }
      AuthorId get_author() const { return author; }
      RequestStatus get_status() const { return status; }
      const std::vector<RequestLine> &get_lines() const { return lines; }

      bool contains(Player_Id id) const {
        return std::any_of(
          lines.begin(), lines.end(),
          [id](const RequestLine &l) { return l.get_id().value == id.value; });
      }

      bool has_Money(const std::string &code) const {
        return std::any_of(
          lines.begin(), lines.end(),
          [&code](const RequestLine &l) { return l.get_Money().get_code() == code; });
      }

      Money total() const {
        Money sum = 0;
        for (const auto &line: lines) {
          auto amount = line.calculate_amount();
          if (amount > limit - sum)
            throw errors::RequestLimitExceeded(id.value, sum.get_kopecks(), amount.get_kopecks(), limit.get_kopecks());
          sum = sum + amount;
        }
        return Money(sum);
      }

      void add_line(Player_Id id, Money Money, Quantity quantity) {
        require_draft();

        if (contains(id))
          throw errors::DuplicatePlayer_Id(id.value, id.value);
        if (has_Money(Money.get_code()))
          throw errors::DuplicateMoney(id.value, Money.get_code());

        RequestLine candidate(id, std::move(Money), quantity);
        ensure_addition_fits(total(), candidate.calculate_amount());
        lines.push_back(std::move(candidate));
      }

      void change_quantity(Player_Id id, Quantity next) {
        require_draft();
        auto it =
            std::find_if(lines.begin(), lines.end(), [id](const RequestLine &l) {
              return l.get_id().value == id.value;
            });

        if (it == lines.end())
          throw errors::LineNotFound(id.value, id.value);

        auto remainder = total() - it->calculate_amount();
        ensure_addition_fits(remainder, it->get_Money().get_price() * next);
        it->change_quantity(next);
      }

      void submit() {
        require_draft();
        if (lines.empty())
          throw errors::EmptyRequest(id.value);
        status = RequestStatus::submitted;
      }

      void cancel(AuthorId actor) {
        if (actor.value != author.value)
          throw errors::CancellationDenied(id.value, actor.value, author.value);
        if (status != RequestStatus::submitted)
          throw errors::RequestNotSubmitted(id.value, static_cast<int>(status));
        status = RequestStatus::cancelled;
      }

      static PurchaseRequest restore(RequestId id, AuthorId author,
                                     RequestStatus status,
                                     const std::vector<RequestLine> &rows) {
        PurchaseRequest request(id, author);
        for (const auto &row: rows)
          request.add_line(row.get_id(), row.get_Money(), row.get_quantity());
        request.status = status;
        return request;
      }
    };
  }