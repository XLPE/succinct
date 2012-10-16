#define BOOST_TEST_MODULE cartesian_tree
#include "test_common.hpp"

#include <cstdlib>
#include <boost/foreach.hpp>

#include "mapper.hpp"
#include "cartesian_tree.hpp"

typedef int64_t value_type;

// XXX test (de)serialization

void test_rmq(std::vector<value_type> const& v, succinct::cartesian_tree const& tree, std::string test_name)
{
    // XXX should we test this case?
    if (v.empty()) return;
    
    std::vector<uint64_t> tests;
    //tests.push_back(0);
    tests.push_back(1);
    for (size_t t = 0; t < 10; ++t) {
        tests.push_back(rand() % v.size());
    }
    
    for(size_t t = 0; t < tests.size(); ++t) {
        uint64_t a = tests[t];
        if (a > v.size()) continue;
        
        uint64_t min_idx = a;
        value_type cur_min = v[a];
        
        BOOST_REQUIRE_EQUAL(min_idx, tree.rmq(a, a));
        
        for (uint64_t b = a + 1; b < v.size(); ++b) {
            if (v[b] < cur_min) {
                cur_min = v[b];
                min_idx = b;
            }
            
            uint64_t found_idx = tree.rmq(a, b);

            MY_REQUIRE_EQUAL(min_idx, found_idx,
			     "rmq (" << test_name << "):"
                             << " a = " << a
                             << " b = " << b
                             << " min = " << cur_min
                             << " found_min = " << v[found_idx]
                             );
        }
    }
}

BOOST_AUTO_TEST_CASE(cartesian_tree)
{
    srand(42);

    {
        std::vector<value_type> v(20000);
        for (size_t i = 0; i < v.size(); ++i) {
            v[i] = i;
        }
        
        succinct::cartesian_tree t(v);
        test_rmq(v, t, "Increasing values");
    }

    {
        std::vector<value_type> v(20000);
        for (size_t i = 0; i < v.size(); ++i) {
            v[i] = v.size() - i;
        }
        
        succinct::cartesian_tree t(v);
        test_rmq(v, t, "Decreasing values");
    }

    {
        std::vector<value_type> v(20000);
        for (size_t i = 0; i < v.size(); ++i) {
            if (i < v.size() / 2) {
                v[i] = i;
            } else {
                v[i] = v.size() - i;
            }
        }
        
        succinct::cartesian_tree t(v);
        test_rmq(v, t, "Unimodal values");
    }

    {
        //size_t sizes[] = {2, 4, 512, 514, 8190, 8192, 8194, 16384, 16386, 100000};
        size_t sizes[] = {512, 514, 8190, 8192, 8194, 16384, 16386, 100000};
        for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
            std::vector<value_type> v(sizes[i]);
            for (size_t i = 0; i < v.size(); ++i) {
                v[i] = rand() % 1024;
            }
        
            succinct::cartesian_tree t(v);
            test_rmq(v, t, "Random values");
        }
    }

    // {
    //     std::vector<char> v;
    //     succinct::bp_vector bitmap(v);
    //     test_rmq(v, bitmap, "Empty vector");
    // }
    
}

