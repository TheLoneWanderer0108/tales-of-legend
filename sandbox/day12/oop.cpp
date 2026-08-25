#include <iostream>
/*Convert `Actor` into a class with private `hp_` and public
      `int Hp() const`, `void TakeDamage(int)`, `void Heal(int)`*/
/*Make it impossible for `hp_` to exceed `maxHp_` or drop below 0 - clamp inside the
      class, not at every call site. This is the entire point of encapsulation*/
/*Write a constructor `Actor(std::string name, Stats stats)` using a member
      initialiser list. Understand why the list is preferred over assigning in the body*/
/*Mark every member function that does not modify state as `const`. Then try to call a
      non-`const` function on a `const Actor&` and read the error*/
/*Add `bool IsAlive() const` and remove the free function version*/
class Stats{
    public:
        int hp{100};
        int maxHP{100};
        int att{1};
        int def{1};
};

class Actor{
    private:
        std::string name{' '};
        Stats stats;
    public:
        Actor(std::string name, Stats stats):
            name{name},
            stats{stats}{}
        int Hp() const{
            return stats.hp;
        }
        void TakeDamage(int damage){
            stats.hp-=damage;
             if(stats.hp <= 0){
                stats.hp = 0;
            }
        }
        void Heal(int healammount){
            stats.hp+=healammount;
            if(stats.hp > stats.maxHP){
                stats.hp = stats.maxHP;
            }
        }
        bool isAlive() const{
           return stats.hp > 0;
        }

};

int main(){
    Actor joe{"joe",{100, 100, 1, 1}};

    joe.Heal(10);
    joe.TakeDamage(20);
    //joe.Heal(50);
    std::cout << joe.Hp();



    return 0;
}