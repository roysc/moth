// -*- coding: utf-8 -*-
// test/parse.cc

#include <exception>
#include <string>
#include <vector>
#include <iterator>
#include <functional>
#include "../util/io.hh"

#include "../parse.hh"

using std::vector;
using std::string;

using DtypeNames = map<string, unsigned>;
DtypeNames dtypes = {
  {"bool", 0},
  {"int", 1},
  {"real", 2},
  {"realm(int)", 3},
  {"realm(real)", 4},
  {"str", 5},
  {"nil", 6}
};

template <class It>
Rule<It> rule_realm(unsigned& out)
{
  using qi::int_;
  using qi::double_;
  using qi::lit;

  std::vector<double> vr;
  std::vector<int> vz;

  // "real [a, b]"
  auto vr_app = [&](double e){vr.push_back(e);};
  auto r_real = lit("real")
    >> '[' >> double_ [vr_app] >> ',' >> double_ [vr_app] >> ']';
  // "int [a, b]"
  auto vz_app = [&](int e){vz.push_back(e);};
  auto r_int = lit("int")
    >> '[' >> int_ [vz_app] >> ',' >> int_ [vz_app] >> ']';

  auto r_rlm =
    r_real [ ([&]{
        out = dtype::ty_rcont;
        // out.set(data::RlmCont{0, vr.at(0), vr.at(1)});
      }) ] |
    r_int  [ ([&]{
        out = dtype::ty_rdisc;
        // out.set(data::RlmDisc{0, vz.at(0), vz.at(1)});
      })];
  return r_rlm;
}

bool parse_data(string str, unsigned& out)
{
  const vector<string> simple_dts{"bool", "int", "real", "str", "nil"};

  using It = string::iterator;
  Rule<It> r_data;

  for (auto&& e: simple_dts) {
    util::println("setting dtype parser: ", e);
      
    auto it = dtypes.find(e);
    if (it == end(dtypes)) throw std::runtime_error(e);

    util::println("e: (", &e[0], ")");
    
    std::function<void()> show;
    show = [=] {
      // util::println("at: ", e);
      // util::println("at: (", &e[0], ")");
    };
    // show = [s{e}] { util::println("at: ", s); };
    // show = std::bind(util::println<string, string>, "at: ", e);
    // r_data = r_data | (qi::lit(e) [show]);
    r_data = r_data | (qi::lit(e) [show]);
  }
  
  // named references parsed
  vector<string> refs;
  auto app_ref = [&](string s){
    // out = Data{dtype::ty_ref};
    refs.push_back(s);
  };
  
  // r_data
  //   |= rule_realm<It>(out)
  //   |= (qi::lit("ref") >> '(' >> r_sym[ app_ref ] >> ')')
  //   ;

  // auto bam = [=] { util::println("bam"); };
  // r_data = qi::lit("real") [bam] | qi::lit("bool") [bam] | qi::lit("int");
  
  auto f = begin(str), l = end(str);
  bool r = phrase_parse(f, l, r_data, ascii::space);
  // bool r = phrase_parse(f, l, qi::lit(), ascii::space);
  
  if (f != l) return {};
  return r;
}

int main()
{
  vector<string> vs = {
    "a", "a.", "a.b", ".a", "a.b.c", "..b.c", "..b.c", "a . b. c  ",
    "a_", "a-", "a0. b", "a", "a_0_1.c",
  };

  for (auto&& e: vs) {
    vector<string> o;
    bool r = parse_cptpath(e, o);
    if (r) util::println('"', e, "\" => ", o);
    else util::println('"', e, "\" => failed");
  }

  {
    // Data d;
    vector<string> ds = {
      "real", "bool", "int", 
    };

    for (auto&& e: ds) {
      unsigned o;
      bool r = parse_data(e, o);
      if (r) util::println('"', e, "\" => ", o);
      else util::println('"', e, "\" => failed");
    }
  }
}
