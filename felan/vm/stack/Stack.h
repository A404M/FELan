//
// Created by a404m on 9/24/22.
//

#ifndef FVM_STACK_H
#define FVM_STACK_H

#include <deque>
#include <stdexcept>

namespace felan {

    template<typename T>
    class Stack {
    public:
        using Holder = std::deque<T>;
        Holder holder{};

        Stack() = default;

        void push(T t);
        T &top();
        void pop();
        void pop(typename Holder::size_type n);
        T getPop();
        template<class... Args>
        void emplace(Args&&... args);

        void clear();
    };
    template<typename T>
    void Stack<T>::push(T t){
        this->holder.template emplace_back(std::move(t));
    }

    template<typename T>
    T &Stack<T>::top() {
        if(this->holder.empty())
            throw std::runtime_error("stack damaged");
        return this->holder.back();
    }

    template<typename T>
    void Stack<T>::pop() {
        if(this->holder.empty())
            throw std::runtime_error("stack damaged");
        holder.pop_back();
    }

    template<typename T>
    template<class... Args>
    void Stack<T>::emplace(Args &&... args) {
        this->holder.template emplace_back(args...);
    }

    template<typename T>
    void Stack<T>::pop(typename Holder::size_type n) {
        if(n > this->holder.size()){
            throw std::runtime_error("broken stack");
        }
        this->holder.erase(this->holder.end()-n,this->holder.end());
    }

    template<typename T>
    void Stack<T>::clear() {
        this->holder.clear();
    }

    template<typename T>
    T Stack<T>::getPop() {
        if(this->holder.empty())
            throw std::runtime_error("broken stack");
        T temp = this->holder.back();
        this->holder.pop_back();
        return temp;
    }

} // felan

#endif //FVM_STACK_H
