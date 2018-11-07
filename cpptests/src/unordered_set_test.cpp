#include <iostream>
#include <unordered_set>

using std::cout;
using std::endl;
using std::unordered_set;

class Foo
{
public:
    Foo(int i) { this->id = i; }

    std::size_t hash() const
    {
        cout << "Hash!" << endl;
        return std::hash<int>()(this->id);
    }
    int id;
};

struct CustomComparator
{
public:
    bool operator()(const Foo* const& a, const Foo*& b) const
    {
        cout << "CustomComparator!" << endl;
        return a->id == b->id;
    }
};

struct barHash
{
public:
    std::size_t operator()(const Foo* const& obj) const { return obj->hash(); }
};

unordered_set<const Foo*, barHash, CustomComparator> mySet;
// unordered_set<const Foo*> mySet;

int main(int argc, char** argv)
{
    for (int i = 0; i < 10; i++) {
        mySet.insert(new Foo(i));
    }

    for (int j = 0; j < 10; j++) {
        auto foo9 = new Foo(9);
        auto entry = mySet.insert(foo9);
        if (!entry.second) {
            cout << "Already present" << endl;
        } else {
            cout << "Not present" << endl;
        }
        delete foo9;
    }

    cout << "SetSize: " << mySet.size() << endl;

    return 0;
}
