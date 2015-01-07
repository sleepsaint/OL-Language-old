//
//  main.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include <iostream>
#include "oljson.h"
#include "olsource.h"

using namespace std;

void test_parse() {
    vector<string> test = {
        "^.wear.{^.person.{~.person}.wear.hat}.price",
        "#(abd,bdf)",
        "(abd, adfsdf, asdfd, ",
        "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12.4) )",
        "!(abd, !@.123.abd.{@.abd}.(~.abc,daf, $12)",
        "!()",
        "!(abd, !@.123.abd.{@.abd}.)",
        "(abd, !@.123.abd.$123.{@.abd})",
        "(abd, sdfe",
        "(abd, !@.123.abd.123.{@.abd.{~.abd})",
        "(abd, !@.123.abd.123.{@.abd.{.abd})",
        "(abd, !)"
    };
    
    for (const auto& i : test) {
        auto value = OL::Source::parse(i.c_str(), i.length());
        if (value) {
            cout << value->description() << endl;
        }
    }
    
}

string root = "{\"person\":{\"P0001\":{\"name\":\"Tom\",\"age\":30,\"wear\":{\"hat\":\"W0001\",\"upper\":\"W0002\",\"under\":\"W0003\",\"shoes\":null}},\"P0002\":{\"name\":\"May\",\"age\":25,\"wear\":{\"hat\":\"W0004\",\"upper\":\"W0005\",\"under\":\"W0006\",\"shoes\":\"W0007\"}}},\"wear\":{\"W0001\":{\"name\":\"Red Hat\",\"price\":100},\"W0002\":{\"name\":\"White Jacket\",\"price\":200},\"W0003\":{\"name\":\"Black Shorts\",\"price\":50},\"W0004\":{\"name\":\"White Hat\",\"price\":210},\"W0005\":{\"name\":\"Red Jacket\",\"price\":220},\"W0006\":{\"name\":\"White Skirt\",\"price\":60},\"W0007\":{\"name\":\"Red HHS\",\"price\":10}}}";

string temp = "{\"person\":\"P0001\",\"person2\":\"^.person.P0001\",\"wearnow\":\"upper\",\"personwear\":\"^.wear.{~.person2.wear.{~.wearnow}}\",\"wearfilter1\":\"#(>, @.price, $150)\",\"wearsorter1\":\"#(!(=,@.name,Red Hat),!@.price))\",\"now\":\"^.wear\"}";

auto root_json = OL::JSON::parse(root.c_str(), root.length());
auto temp_json = OL::JSON::parse(temp.c_str(), temp.length());

void test_lookup() {
    using namespace OL;
    vector<string> test = {
        "^.wear.{^.person.{~.person}.wear.hat}.price",
        "^.wear.{~.person2.wear.hat}.price",
        "~.personwear.price",
        "(-, (+, ^.wear.W0001.price, ^.wear.W0002.price), ^.wear.W0002.price)",
        "(filter, ^.wear, #(>, @.price, $150))",
        "(filter, ^.wear, ~.wearfilter1)",
        "(or, !(>,^.wear.W0001.price,50), (>, ^.wear.W0002.price, 100) )",
        "(sort, ^.wear, #(@.price))",
        "(sort, ^.wear, #(!(=,@.name,Red Hat),@.price))",
        "(sort, ^.wear, ~.wearsorter1)",
        "(some, ^.wear, ~.wearfilter1)",
        "(sort, ~.now, ~.wearsorter1)",
        "(some, ~.now, #(>,@.price,50))",
        "(sort, ^.wear, #@.price)",
        "(sort, ^.wear, #!@.price)",
        "(random)",
        "(random, $5)",
        "(random, $-5, $-3)"
    };
    for (const auto& i : test) {
        auto value = Source::parse(i.c_str(), i.length());
        auto value2 = value->lookup(root_json, temp_json, root_json);
//        if (value2) {
//            cout << i << endl;
//            cout << value2->description() << endl;
//
//        }
    }
    
}

void test_parse_json() {
    auto json = OL::JSON::parse(root.c_str(), root.length());
    cout << json->description() << endl;
}

typedef void (*FUNC)();

void PP(FUNC func) {
    auto start = clock();
    for (int i = 0; i < 1000; ++i) {
        func();
    }
    auto end = clock();
    cout << end - start << endl;
}

int main(int argc, const char * argv[]) {
  
//        test_parse();
    PP(test_lookup);
//    test_parse_json();
    return 0;
}
