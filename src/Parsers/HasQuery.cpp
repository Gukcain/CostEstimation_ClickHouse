#include <Parsers/HasQuery.h>

bool DB::HasQuery::hasquery = false;
std::mutex DB::HasQuery::mutexofall;
// DB::HasQuery::HasQuery() noexcept;
// DB::HasQuery::HasQuery(bool has){hasquery = has;}


