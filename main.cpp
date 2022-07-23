#include <iostream>
#include <cmath>
#include <vector>

class AutoDiffFunction {
    public:
        virtual float forward(float x) =0;
        virtual float backward(float x) =0;
};

class IdentityFunction : public AutoDiffFunction {
    float forward(float x) {
        return x;
    }

    float backward(float x) {
        return 1.0f;
    }
};

class SinFunction : public AutoDiffFunction {

    public:
        SinFunction(AutoDiffFunction* arg) {
            m_arg = arg;
        }

    float forward(float x) {
        return std::sin(m_arg->forward(x));
    }

    float backward(float x) {
        float eval = std::cos(m_arg->forward(x));
        return eval * m_arg->backward(x);
    }

    private:
        AutoDiffFunction* m_arg;
};

class PolynomialFunction : public AutoDiffFunction {
    public:
        PolynomialFunction(AutoDiffFunction* arg, int exp) {
            m_arg = arg;
            m_exp = exp;
        }

        float forward(float x) {
            float val = m_arg->forward(x);

            float val_tmp = 1.0f;

            for (int i = 0; i < m_exp; i++) {
                val_tmp *= val;
            }

            return val_tmp;
        }

        float backward(float x) {
            float val = m_arg->forward(x);
            float val_tmp = 1.0f;

            for (int i = 0; i < m_exp - 1; i++) {
                val_tmp *= val;
            }

            return m_exp * val_tmp * m_arg->backward(x);
        }

    private:
        AutoDiffFunction* m_arg;
        int m_exp;
};

class SqrtFunction : public AutoDiffFunction {

    public:
        SqrtFunction(AutoDiffFunction* arg) {
            m_arg = arg;
        }

        float forward(float x) {
            return std::sqrt(m_arg->forward(x));
        }

        float backward(float x) {
            return (1.0f / (2.0f * forward(x))) * m_arg->backward(x);
        }

    private:
        AutoDiffFunction* m_arg;
};

class LogFunction : public AutoDiffFunction {

    public:
        LogFunction(AutoDiffFunction* arg) {
            m_arg = arg;
        }

    float forward(float x) {
        return std::log(m_arg->forward(x));
    }

    float backward(float x) {
        return (1.0f / m_arg->forward(x)) * m_arg->backward(x);
    }

    private:
        AutoDiffFunction* m_arg;
};

class AddFunction : public AutoDiffFunction {
    
    public:
        AddFunction(AutoDiffFunction* func1, AutoDiffFunction* func2) {
            m_func1 = func1;
            m_func2 = func2;
        }
    
    float forward(float x) {
        return m_func1->forward(x) + m_func2->forward(x);
    }

    float backward(float x) {
        return m_func1->backward(x) + m_func2->backward(x);
    }

    private:
        AutoDiffFunction* m_func1;
        AutoDiffFunction* m_func2;
};

class DivFunction : public AutoDiffFunction {
    
    public:
        DivFunction(AutoDiffFunction* func1, AutoDiffFunction* func2) {
            m_func1 = func1;
            m_func2 = func2;
        }
    
    float forward(float x) {
        return m_func1->forward(x) / m_func2->forward(x);
    }

    float backward(float x) {
        return (m_func1->backward(x) * m_func2->forward(x) - m_func1->forward(x) * m_func2->backward(x)) / (m_func2->forward(x) * m_func2->forward(x));
    }

    private:
        AutoDiffFunction* m_func1;
        AutoDiffFunction* m_func2;
};

class ExpFunction : public AutoDiffFunction {
    public:
        ExpFunction(AutoDiffFunction* arg) {
            m_arg = arg;
        }

        float forward(float x) {
            return std::exp(m_arg->forward(x));
        }

        float backward(float x) {
            return forward(x) * m_arg->backward(x);
        }

    private:
        AutoDiffFunction* m_arg;
};

class ConstFunction : public AutoDiffFunction {
    public:
        ConstFunction(float constant) {
            m_const = constant;
        }

        float forward(float x) {
            return m_const;
        }

        float backward(float x) {
            return 0;
        }

    private:
        float m_const;
};

class FunctionFactory {
    public:
        static AutoDiffFunction* add(AutoDiffFunction* f1, AutoDiffFunction* f2) { return new AddFunction(f1, f2); }
        static AutoDiffFunction* div(AutoDiffFunction* f1, AutoDiffFunction* f2) { return new DivFunction(f1, f2); }


        static AutoDiffFunction* log(AutoDiffFunction* arg) { return new LogFunction(arg); }
        static AutoDiffFunction* sin(AutoDiffFunction* arg) { return new SinFunction(arg); }
        static AutoDiffFunction* sqrt(AutoDiffFunction* arg) { return new SqrtFunction(arg); }
        static AutoDiffFunction* pow(AutoDiffFunction* arg, int n) { return new PolynomialFunction(arg, n); }
        static AutoDiffFunction* exp(AutoDiffFunction* arg) {return new ExpFunction(arg);}
        static AutoDiffFunction* x() { return new IdentityFunction(); }
        static AutoDiffFunction* c(float c) {return new ConstFunction(c); }
};

int main(int argc, char** argv) {

    AutoDiffFunction* func = FunctionFactory::sin(
        FunctionFactory::div(
            FunctionFactory::sqrt(FunctionFactory::add(FunctionFactory::exp(FunctionFactory::x()), FunctionFactory::pow(FunctionFactory::x(), 2))),
            FunctionFactory::c(2.0f)
        )
    );

    std::vector<float> x_vals{};

    for (float s = 0.0; s <= 5.0; s += 0.05) {
        std::cout << s << ", ";
        x_vals.push_back(s);
    }
    
    std::cout << "\n\n";

    for(auto& x : x_vals) {
        float y = func->forward(x);
        std::cout << y << ", ";
    }

    std::cout << "\n\n";

    for(auto& x : x_vals) {
        float deriv = func->backward(x);
        std::cout << deriv << ", ";
    }


    return 0;
}