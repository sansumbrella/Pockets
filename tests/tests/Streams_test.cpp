//
//  Streams_test.cpp
//
//  Created by David Wicks on 9/24/15.
//  Copyright © 2015 David Wicks. All rights reserved.
//

#include "catch.hpp"
#include <iostream>

using namespace std;

TEST_CASE("String Streams are kind of okay")
{
  SECTION("String streams can parse space-separated values")
  {
    auto ssv = "1 2 3";

    stringstream ss(ssv);
    int a, b, c;
    ss >> a >> b >> c;

    REQUIRE(a == 1);
    REQUIRE(b == 2);
    REQUIRE(c == 3);
  }

  SECTION("String streams can parse comma-separated values through really ugly ctype facet use. Yeesh.")
  {
    auto csv = "1, 2, 3";

    // ctype derivation and locale imbueing taken from a stack overflow answer.
    // http://stackoverflow.com/questions/19107439/c-getline-multiple-variable-types-using-comma-as-delimiter
    // pretty sure I never would have imagined this on my own.
    class csv_char : public std::ctype<char>
    {
    public:
      static const mask* make_table()
      {
        static auto v = std::vector<mask>(classic_table(), classic_table() + table_size);
        v[','] |= space;
        return &v[0];
      }

      csv_char(std::size_t refs = 0)
      : ctype(make_table(), false, refs)
      {}
    };

    stringstream ss(csv);
    // the locale takes ownership of the csv_char pointer
    ss.imbue(std::locale(ss.getloc(), new csv_char));
    int a, b, c;
    ss >> a >> b >> c;

    cout << "a " << a << ", b " << b << ", c " << c << endl;

    REQUIRE(a == 1);
    REQUIRE(b == 2);
    REQUIRE(c == 3);
  }

  SECTION("getline() can parse CSVs in a neater style with less obfuscation.")
  {
    auto csv =
R"(1, 2, 3
   4, 5, 6,
   7, 8, 9
)";
    auto ss = stringstream(csv);

    auto line = string();
    while (getline(ss, line))
    {
      auto lss = stringstream(line);
      auto token = string();
      auto tokens = stringstream();
      while (getline(lss, token, ','))
      {
        tokens << token;
      }

      int x, y, z;
      tokens >> x >> y >> z;
      cout << "Line: " << x << ", " << y << ", " << z << endl;
    }
  }

}
