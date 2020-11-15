#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <regex>
using namespace std;
/////////////////////////////////////////////////////////
shared_ptr<QueryBase> QueryBase::factory(const string &s)
{
  istringstream istream(s);

  regex regex_WORD_Function("^\\s*([\\w']+)\\s*$");
  regex regex_AND_Function("^\\s*(AND)\\s+([\\w']+)\\s+([\\w']+)\\s*$");
  regex regex_OR_Function("^\\s*(OR)\\s+([\\w']+)\\s+([\\w']+)\\s*$");
  regex regex_AD_Function("^\\s*(AD)\\s+([\\w']+)\\s+([\\w']+)\\s*$");

  vector<string> ans((istream_iterator<string>(istream)), istream_iterator<string>());

  if (regex_match(s, regex_WORD_Function))
    return std::shared_ptr<QueryBase>(new WordQuery(ans[0]));

  else if (regex_match(s, regex_AND_Function))
    return std::shared_ptr<QueryBase>(new AndQuery(ans[1], ans[2]));

  else if (regex_match(s, regex_OR_Function))
    return std::shared_ptr<QueryBase>(new OrQuery(ans[1], ans[2]));

  else if (regex_match(s, regex_AD_Function))
    return std::shared_ptr<QueryBase>(new AdjacentQuery(ans[1], ans[2]));

  else
    throw invalid_argument("Unrecognized search");

  QueryResult AndQuery::eval(const TextQuery &text) const
  {
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();
    set_intersection(left_result.begin(), left_result.end(),
                     right_result.begin(), right_result.end(),
                     inserter(*ret_lines, ret_lines->begin()));
    return QueryResult(rep(), ret_lines, left_result.get_file());
  }

  QueryResult OrQuery::eval(const TextQuery &text) const
  {
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>(left_result.begin(), left_result.end());
    ret_lines->insert(right_result.begin(), right_result.end());
    return QueryResult(rep(), ret_lines, left_result.get_file());
  }
  /////////////////////////////////////////////////////////
  QueryResult AdjacentQuery::eval(const TextQuery &text) const
  {
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();



    return QueryResult(rep(), ret_lines, left_result.get_file());
  }
  /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////
  std::ostream &print(std::ostream & os, const QueryResult &qr)
  {
    os << "\"" << qr.sought << "\""
       << " occurs " << qr.lines->size() << " times:" << std::endl;
    for (auto num : *qr.lines)
    {
      os << "\t(line " << num + 1 << ") "
         << *(qr.file->begin() + num) << std::endl;
    }
    return os;
  }
  /////////////////////////////////////////////////////////