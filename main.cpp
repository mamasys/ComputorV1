#include "main.hpp"

std::tuple<std::complex<double>, std::complex<double>, double> calc_reduced_2(std::string eq) {
    std::istringstream iss(eq);
    std::vector<std::string> tokens{
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    double a = 0.0, b = 0.0, c = 0.0;
    for (size_t i = 0; i + 2 < tokens.size(); ++i) {
        if (tokens[i+1] == "*" && tokens[i+2] == "X^2")
            a = std::stod(tokens[i]);
        else if (tokens[i+1] == "*" && tokens[i+2] == "X^1")
            b = std::stod(tokens[i]);
        else if (tokens[i+1] == "*" && tokens[i+2] == "X^0")
            c = std::stod(tokens[i]);
    }

    double D = b * b - 4 * a * c;

    std::complex<double> sqrtD = std::sqrt(std::complex<double>(D, 0.0));

    std::complex<double> r1 = (-b - sqrtD) / (2.0 * a);
    std::complex<double> r2 = (-b + sqrtD) / (2.0 * a);

    return {r1, r2, D};
}

std::pair<std::string, int> reduced(std::string equation_reduced) {
    int degree = 0;
        for (int i = 0; i < equation_reduced.length(); ++i) {
        if (equation_reduced[i] == '+' && i > 0 && equation_reduced[i-1] == '+') {
            equation_reduced.erase(i, 1);
            --i;
        }
        else if (equation_reduced[i] == '-' && i > 0 && equation_reduced[i-1] == '-') {
            equation_reduced.replace(i-1, 2, "+");
            --i;
        }
        else if (equation_reduced[i] == '-' && i > 0 && equation_reduced[i-1] == '+') {
            equation_reduced.replace(i-1, 2, "-");
            --i;
        }
        else if (equation_reduced[i] == '+' && i > 0 && equation_reduced[i-1] == '-') {
            equation_reduced.replace(i-1, 2, "-");
            --i;
        }
    }
    if (equation_reduced[0] == '+') {
        equation_reduced.erase(0, 1);
    }
    
    int pos = equation_reduced.find("  ");
    while (pos != std::string::npos) {
        equation_reduced.replace(pos, 2, " ");
        pos = equation_reduced.find("  ");
    }
    equation_reduced.erase(0, equation_reduced.find_first_not_of(" "));
    equation_reduced.erase(equation_reduced.find_last_not_of(" ") + 1);
    
    std::map<int, double> terms;
    std::string term;
    int i = 0;
    bool left_side = true;
    
    while (i < equation_reduced.length()) {
        if (equation_reduced[i] == '=') {
            left_side = false;
            ++i;
            continue;
        }
        term.clear();
        while (i < equation_reduced.length() && equation_reduced[i] != '+' && equation_reduced[i] != '-' && equation_reduced[i] != '=') {
            term += equation_reduced[i];
            ++i;
        }
        char next_sign = (i < equation_reduced.length()) ? equation_reduced[i] : '\0';
        
        double coeff = 1.0;
        bool negative = term[0] == '-';
        if (negative || term[0] == '+') term.erase(0, 1);
        
        int star_pos = term.find('*');
        if (star_pos != std::string::npos) {
            std::string coeff_str = term.substr(0, star_pos);
            coeff_str.erase(coeff_str.find_last_not_of(" ") + 1);
            coeff_str.erase(0, coeff_str.find_first_not_of(" "));
            if (!coeff_str.empty()) coeff = std::stod(coeff_str);
        }
        if (negative) coeff = -coeff;
        
        int exp = 0;
        auto posX = term.find('X');
        if (posX != std::string::npos) {
            auto caret = term.find('^', posX);
            if (caret != std::string::npos)
                exp = std::stoi(term.substr(caret + 1));
            else
                exp = 1;
        }
        degree = std::max(degree, exp);
        if (left_side)
            terms[exp] += coeff;
        else
            terms[exp] -= coeff;
        if (next_sign == '+' || next_sign == '-') 
            ++i;
    }
    
    std::string final_equation;
    bool first = true;
    for (auto it = terms.rbegin(); it != terms.rend(); ++it) {
        if (it->second != 0) {
            if (!first && it->second > 0) final_equation += " + ";
            else if (!first && it->second < 0) final_equation += " - ";
            else if (it->second < 0) final_equation += "-";
            final_equation += std::to_string(std::abs(it->second)) + " * X^" + std::to_string(it->first);
            first = false;
        }
    }
    if (final_equation.empty()) final_equation = "0";
    final_equation += " = 0";
    return std::make_pair(final_equation, degree);
}

int main(int c, char **a) {
    std::string signs[] = {"*", "^", "-", "=", "+", "X"};
    if (c != 2)
        return (std::cerr << "Invalid usage" << std::endl, 1);
    std::string equation = a[1];
    if (_DEBUG)
        std::cout << "[DEBUG] output : " << equation << "\n";
    for (char ch : equation) {
        if (ch == ' ' || isdigit(ch) || ch == '.') continue;
        bool valid = false;
        for (const auto& sign : signs) {
            if (ch == sign[0]) {
                valid = true;
                break;
            }
        }
        if (!valid)
            return (std::cerr << "Invalid character: " << ch << std::endl, 1);
    }

    std::string equation_reduced = equation;
    auto [eq, deg] = reduced(equation);
    std::cout << "Reduced equation: " << eq << "\n";
    std::cout << "Polynomial degree: " << deg << "\n";

    if (deg == 0) {
        double c = std::stod(eq);
        if (c == 0.0)
            return (std::cout << "Infinite solutions." << std::endl, 0);
        else
            return (std::cout << "No solution." << std::endl, 0);
    }
    if (deg == 1) {
        std::istringstream iss(eq);
        double b, c;
        char sign;
        std::string tmp;
        iss >> b >> tmp >> tmp >> sign >> c >> tmp >> tmp;
        if (sign == '-') c = -c;
        return (std::cout << "The solution is:\n" << -c / b << std::endl, 0);
    }
    if (deg > 2)
        std::cout << "The polynomial degree is strictly greater than 2, I can't solve." << std::endl;
    auto [r1, r2, D] = calc_reduced_2(eq);
    if (_DEBUG)
        std::cout << "[DEBUG] Discriminant: " << D << "\n";
    if (D > 0) {
        std::cout << "Discriminant is strictly positive, the two solutions are:\n" << r1.real() << "\n" << r2.real() << "\n";
    }
    else if (D == 0) {
        std::cout << "Discriminant is strictly positive, the two solutions are:\n" << r1.real() << "\n";
    }
    else {
        std::cout << "Discriminant is strictly negative, the two complex solutions are:\n" << r1 << "\n" << r2 << "\n";
    }
}
