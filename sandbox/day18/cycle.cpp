#include <iostream>
#include <vector>
#include <memory>
struct NodeA;
struct NodeB
{
    std::weak_ptr<NodeA> pointer2;
};

struct NodeA
{
    std::shared_ptr<NodeB> pointer;
};



int main(){

    auto nodeA = std::make_shared<NodeA>();
    auto nodeB = std::make_shared<NodeB>();

    nodeA ->pointer = nodeB;
    nodeB ->pointer2 = nodeA;

    return 0;
}