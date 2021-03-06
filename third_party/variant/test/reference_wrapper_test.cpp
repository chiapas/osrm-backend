#include <cstdlib>
#include <functional>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include <mapbox/variant.hpp>

using namespace mapbox;

namespace test {

struct point
{
  public:
    point(double x_, double y_)
        : x(x_), y(y_) {}
    double x;
    double y;
};

struct line_string : std::vector<point>
{
};
struct polygon : std::vector<line_string>
{
};
using variant = util::variant<std::reference_wrapper<const point>,
                              std::reference_wrapper<const line_string>,
                              std::reference_wrapper<const polygon>>;

struct print
{
    using result_type = void;
    void operator()(point const& pt) const
    {
        std::cerr << "Point(" << pt.x << "," << pt.y << ")" << std::endl;
    }
    void operator()(line_string const& line) const
    {
        std::cerr << "Line(";
        bool first = true;
        for (auto const& pt : line)
        {
            if (!first) std::cerr << ",";
            std::cerr << pt.x << " " << pt.y;
            if (first) first = false;
        }
        std::cerr << ")" << std::endl;
    }
    template <typename T>
    void operator()(T const& val) const
    {
        std::cerr << typeid(T).name() << std::endl;
    }
};
}

int main()
{
    std::cerr << sizeof(test::polygon) << std::endl;
    std::cerr << sizeof(test::variant) << std::endl;
    test::point pt(123, 456);
    test::variant var = std::cref(pt);
    util::apply_visitor(test::print(), var);
    test::line_string line;
    line.push_back(pt);
    line.push_back(pt);
    line.push_back(test::point(999, 333));
    var = std::cref(line);
    util::apply_visitor(test::print(), var);
    std::cerr << "Is line (cref) ? " << var.is<std::reference_wrapper<test::line_string const>>() << std::endl;
    auto const& line2 = var.get<test::line_string>(); // accessing underlying type of std::reference_wrapper<T>
    test::print printer;
    printer(line2);
    return EXIT_SUCCESS;
}
