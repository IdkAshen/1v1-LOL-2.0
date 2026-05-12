#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>

const int MAP_SIZE = 20;

struct Weapon {
    std::string name;
    int damage;
};

class Player {
public:
    std::string name;
    int hp = 100;
    int shield = 0;
    int x = 0;
    int y = 0;
    int wood = 0;
    Weapon weapon = {"Pickaxe", 10};
    bool is_bot = false;
    bool alive = true;

    void take_damage(int amount) {
        if (shield >= amount) {
            shield -= amount;
        } else {
            amount -= shield;
            shield = 0;
            hp -= amount;
        }
        if (hp <= 0) {
            hp = 0;
            alive = false;
        }
    }
};

void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int get_distance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

int main() {
    std::srand(std::time(nullptr));

    // Weapons Pool
    std::vector<Weapon> loot_pool = {
        {"Common Assault Rifle", 20},
        {"Rare SMG", 15},
        {"Epic Pump Shotgun", 45},
        {"Legendary SCAR", 35}
    };

    // Instantiate Player
    Player human;
    human.name = "You (ProCoder)";
    human.is_bot = false;

    // Instantiate a 99-Bot Lobby
    std::vector<Player> lobby;
    for (int i = 1; i <= 99; ++i) {
        Player bot;
        bot.name = "Bot_" + std::to_string(i);
        bot.x = std::rand() % MAP_SIZE;
        bot.y = std::rand() % MAP_SIZE;
        bot.shield = (std::rand() % 2) * 50; 
        bot.weapon = loot_pool[std::rand() % loot_pool.size()];
        bot.is_bot = true;
        lobby.push_back(bot);
    }

    int storm_radius = MAP_SIZE;
    int center_x = MAP_SIZE / 2;
    int center_y = MAP_SIZE / 2;

    std::cout << "=== WELCOME TO FORTNITE (C++ EDITION) ===\n";
    std::cout << "🚎 The Battle Bus is flying over a " << MAP_SIZE << "x" << MAP_SIZE << " grid island...\n\n";
    sleep_ms(1000);

    // Drop Phase
    std::cout << "Enter your landing coordinates (X Y) from 0 to " << MAP_SIZE - 1 << ": ";
    std::cin >> human.x >> human.y;
    human.x = std::clamp(human.x, 0, MAP_SIZE - 1);
    human.y = std::clamp(human.y, 0, MAP_SIZE - 1);

    // Loot Phase
    std::cout << "\n📦 Landing... Opening a chest!\n";
    sleep_ms(1500);
    human.weapon = loot_pool[std::rand() % loot_pool.size()];
    human.shield = 100; 
    human.wood = 30;     
    std::cout << "Found: " << human.weapon.name << " (" << human.weapon.damage << " Dmg)!\n";
    std::cout << "Found: 2x Shield Pots (+100 Shield)!\n";
    std::cout << "Found: Materials (+30 Wood)!\n";
    sleep_ms(1500);

    // Main Game Loop
    while (human.alive) {
        // Count active survivors
        long alive_count = std::count_if(lobby.begin(), lobby.end(), [](const Player& b){ return b.alive; }) + 1;
        
        if (alive_count == 1) break; // Only human left

        std::cout << "\n================================================\n";
        std::cout << "👑 SURVIVORS: " << alive_count << " / 100\n";
        std::cout << "💖 HP: " << human.hp << " | 🛡️ SHIELD: " << human.shield << " | 🪵 WOOD: " << human.wood << "\n";
        std::cout << "📍 LOCATION: (" << human.x << ", " << human.y << ") | 🔫 WEAPON: " << human.weapon.name << "\n";
        std::cout << "⛈️ STORM EYE: Radius " << storm_radius << " around (" << center_x << ", " << center_y << ")\n";
        std::cout << "================================================\n";
        sleep_ms(1000);

        // 1. Storm Logic
        if (get_distance(human.x, human.y, center_x, center_y) > storm_radius) {
            std::cout << "⛈️ You are in the STORM! Taking 20 damage!\n";
            human.take_damage(20);
            if (!human.alive) break;
        }

        // Simulating off-screen bot vs bot eliminations to decrease lobby size fast
        int passive_eliminations = std::rand() % 15 + 5; 
        for (auto& bot : lobby) {
            if (bot.alive && passive_eliminations > 0 && std::rand() % 3 == 0) {
                bot.alive = false;
                passive_eliminations--;
            }
        }

        // 2. Turn Choices
        std::cout << "\nWhat is your next play?\n";
        std::cout << "1. Move toward the Safe Zone\n";
        std::cout << "2. Farm Wood (+20 Wood)\n";
        std::cout << "3. Hunt for nearby players\n";
        std::cout << "Choice: ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            if (human.x < center_x) human.x++; else if (human.x > center_x) human.x--;
            if (human.y < center_y) human.y++; else if (human.y > center_y) human.y--;
            std::cout << "🏃 You rotated toward the safe zone center. Now at (" << human.x << ", " << human.y << ")\n";
        } else if (choice == 2) {
            human.wood += 20;
            std::cout << "🪓 You hit some trees. Total Wood: " << human.wood << "\n";
        } else {
            std::cout << "👀 Scanning the horizon for movement...\n";
        }
        sleep_ms(1000);

        // 3. Dynamic Combat Encounter
        Player* enemy = nullptr;
        for (auto& bot : lobby) {
            if (bot.alive && get_distance(human.x, human.y, bot.x, bot.y) <= 3) {
                enemy = &bot;
                break;
            }
        }

        if (enemy) {
            std::cout << "\n❗ SPOTTED: " << enemy->name << " is pushing your position!\n";
            sleep_ms(1000);

            // Building Mechanic
            if (human.wood >= 10) {
                human.wood -= 10;
                std::cout << "🪵 You quickly built a wooden wall! (Absorbs enemy's first strike)\n";
                std::cout << "🔫 You counter-attack with your " << human.weapon.name << "!\n";
                enemy->take_damage(human.weapon.damage);
                std::cout << "💥 Hit! " << enemy->name << " HP is down to " << enemy->hp << "\n";
            } else {
                std::cout << "❌ Out of materials! Direct shootout!\n";
                std::cout << "🔫 You shoot your " << human.weapon.name << "!\n";
                enemy->take_damage(human.weapon.damage);
                
                std::cout << "💥 " << enemy->name << " lasers you back with " << enemy->weapon.name << "!\n";
                human.take_damage(enemy->weapon.damage);
            }

            if (!enemy->alive) {
                std::cout << "💀 ELIMINATED! You knocked out " << enemy->name << " and took their shield cells!\n";
                human.shield = std::min(100, human.shield + 50);
            }
            sleep_ms(1500);
        } else {
            std::cout << "💨 Quiet turn. No nearby firefights found.\n";
            // Bots migrate to center
            for (auto& bot : lobby) {
                if (bot.alive) {
                    if (bot.x < center_x) bot.x++; else bot.x--;
                }
            }
        }

        // 4. Shrink Storm Circle
        if (storm_radius > 2) {
            storm_radius -= 2;
            std::cout << "\n⚡ The Storm is shrinking! Safe zone is getting tighter! ⚡\n";
        }
        sleep_ms(1000);
    }

    // End Game Results
    std::cout << "\n================================================\n";
    if (human.alive) {
        std::cout << "🎉 🎉 🎉 #1 VICTORY ROYALE! 🎉 🎉 🎉\n";
        std::cout << "You conquered the 100-player lobby!\n";
    } else {
        std::cout << "☠️ You were eliminated. Better luck next match! ☠️\n";
    }
    std::cout << "================================================\n";

    return 0;
}
