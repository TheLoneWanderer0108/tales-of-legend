#include <iostream>
#include <vector>
#include <memory>
/*Convert the `Skill` list to `std::vector<std::unique_ptr<Skill>>` properly, using
      `std::make_unique`*/


      struct Stats{
        int hp{100};
        int maxHP{100};
        int att{1};
        int def{1};
        int maxfp{10};
        int fp{10};
};

struct Actor{
    std::string name{' '};
    Stats stats;
};

class Skill{

    public:
        virtual void Execute(Actor& user, Actor& target) = 0;
        virtual std::string Name() const = 0;
        virtual ~Skill() = default;


};

class Attack : public Skill{

    public:
        void Execute(Actor& user, Actor& target) override{
            int damage = user.stats.att * user.stats.att / (user.stats.att + target.stats.def);
            target.stats.hp -= damage;
            std::cout << " User: " << user.name << " Attacks: " << target.name << " For: " << damage << "\n";
            std::cout << "Current hp: " <<target.stats.hp <<"\n";
        }
        std::string Name() const override{
            return "Attack";
        }

};

class FireBall : public Skill{

    public:
    int fireballDam = 5;
    void Execute(Actor& user, Actor& target) override{
        user.stats.fp -= 5;
        int damage = fireballDam * user.stats.maxfp / (user.stats.maxfp + (target.stats.def/2));
        target.stats.hp -= damage;
        std::cout << " User: " <<user.name << " Attacks: " << target.name << "For: " << damage << "\n";
        std::cout << "Current hp: " <<target.stats.hp <<"\n";
    }
    std::string Name() const override{
        return "Fireball";
    }

};

class Heal : public Skill{
    public:
    int healamount = 2;
    void Execute(Actor& user, Actor& target) override{
        int healed = healamount * user.stats.maxfp / 2;
        target.stats.hp += healed;

        std::cout << " User: " << user.name << " Has healed: "<< target.name << " for: " << healed <<"\n";
        std::cout << "target current health: " <<target.stats.hp<<"\n";
    }

    std::string Name() const override{
        return "Heal";
    }

};

void takSkill(const Skill* skill){
    skill -> Name();
}
int main(){
    std::vector<Actor> actors = {{"joe", {100, 100, 1, 1, 2, 2}}, {"darkus", {200, 200, 2, 2, 2, 2}}};
    std::vector<std::unique_ptr<Skill>> skills{};

    std::unique_ptr<Attack> attack = std::make_unique<Attack>();
    std::unique_ptr<FireBall> fireball = std::make_unique<FireBall>();
    std::unique_ptr<Heal> heal = std::make_unique<Heal>();


    skills.push_back(std::move(attack));
    skills.push_back(std::move(fireball));
    skills.push_back(std::move(heal));

    for(size_t i{0}; i < skills.size(); i++){
        skills[i] -> Execute(actors[0], actors[1]);
        takSkill(skills[i].get());
    }

    return 0;
}