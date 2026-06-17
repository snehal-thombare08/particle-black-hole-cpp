#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <algorithm>

const int WIDTH = 1200, HEIGHT = 800;
const int NUM_PARTICLES = 8000;
const float EVENT_HORIZON = 18.f;
const float SPAWN_RADIUS = 350.f;

float randF(float lo, float hi){ return lo+(hi-lo)*(rand()/(float)RAND_MAX); }

struct Particle {
    float x, y, vx, vy;
    float life; // 0=dead, 1=alive
    sf::Color color;

    void reset(float bhx, float bhy) {
        float angle = randF(0, 6.2831f);
        float r = randF(SPAWN_RADIUS * 0.4f, SPAWN_RADIUS);
        x = bhx + std::cos(angle) * r;
        y = bhy + std::sin(angle) * r;

        // Orbital velocity (perpendicular to radius, scaled for orbit)
        float speed = randF(40.f, 90.f);
        vx = -std::sin(angle) * speed;
        vy =  std::cos(angle) * speed;

        // Add small random perturbation
        vx += randF(-15.f, 15.f);
        vy += randF(-15.f, 15.f);

        life = 1.f;
        color = sf::Color(100, 160, 255, 200);
    }
};

std::vector<Particle> particles;

sf::Color particleColor(float dist, float speed) {
    // Far + slow = cool blue, close + fast = hot white/orange
    float tDist = std::max(0.f, std::min(1.f, 1.f - (dist - EVENT_HORIZON) / (SPAWN_RADIUS * 0.6f)));
    float tSpeed = std::max(0.f, std::min(1.f, speed / 600.f));
    float t = tDist * 0.6f + tSpeed * 0.4f;

    if(t < 0.3f) {
        float s = t / 0.3f;
        return sf::Color((uint8_t)(80+s*100),(uint8_t)(140+s*80),(uint8_t)(220+s*35),(uint8_t)(200+s*55));
    } else if(t < 0.6f) {
        float s = (t - 0.3f) / 0.3f;
        return sf::Color(
            (uint8_t)(100 + s * 155),
            (uint8_t)(180 - s * 60),
            (uint8_t)(255 - s * 180),
            (uint8_t)(200 + s * 40));
    } else {
        float s = (t - 0.6f) / 0.4f;
        return sf::Color(
            255,
            (uint8_t)(120 - s * 80),
            (uint8_t)(75 - s * 50),
            255);
    }
}

int main(){
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH,(unsigned)HEIGHT}),
        "Particle Black Hole | LMB: Move Black Hole | RMB: Repulse | Space: Nova | +/-: Mass");
    window.setFramerateLimit(60);

    srand(42);

    float bhx = WIDTH * 0.5f, bhy = HEIGHT * 0.5f;
    float bhMass = 28000.f;
    bool dragging = false;

    // Trail render texture for glow effect
    sf::RenderTexture trailTex({(unsigned)WIDTH, (unsigned)HEIGHT});
    trailTex.clear(sf::Color::Black);
    sf::Sprite trailSprite(trailTex.getTexture());

    sf::RectangleShape fadeRect({(float)WIDTH, (float)HEIGHT});
    fadeRect.setFillColor(sf::Color(0, 0, 2, 5));

    particles.resize(NUM_PARTICLES);
    for(auto& p : particles) p.reset(bhx, bhy);

    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text hud(font);
    hud.setCharacterSize(14);
    hud.setFillColor(sf::Color::White);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(1.5f);
    hud.setPosition({10.f, 10.f});

    sf::Clock clock;
    int aliveCount = NUM_PARTICLES;

    while(window.isOpen()){
        float dt = std::min(clock.restart().asSeconds(), 0.02f);
        auto mpos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse((float)mpos.x, (float)mpos.y);

        bool repulse = false;

        while(auto ev = window.pollEvent()){
            if(ev->is<sf::Event::Closed>()) window.close();
            if(auto* k = ev->getIf<sf::Event::KeyPressed>()){
                if(k->code == sf::Keyboard::Key::Escape) window.close();
                if(k->code == sf::Keyboard::Key::Space){
                    // Nova burst — scatter all particles outward
                    for(auto& p : particles){
                        float dx = p.x - bhx, dy = p.y - bhy;
                        float d = std::sqrt(dx*dx+dy*dy) + 1.f;
                        p.vx += (dx/d) * 800.f;
                        p.vy += (dy/d) * 800.f;
                    }
                }
                if(k->code == sf::Keyboard::Key::Equal) bhMass = std::min(80000.f, bhMass + 3000.f);
                if(k->code == sf::Keyboard::Key::Hyphen) bhMass = std::max(5000.f, bhMass - 3000.f);
                if(k->code == sf::Keyboard::Key::R){
                    for(auto& p : particles) p.reset(bhx, bhy);
                }
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()){
                if(mb->button == sf::Mouse::Button::Left) dragging = true;
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonReleased>()){
                if(mb->button == sf::Mouse::Button::Left) dragging = false;
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()){
                if(mb->button == sf::Mouse::Button::Right) repulse = true;
            }
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) repulse = true;
        if(dragging){ bhx = mouse.x; bhy = mouse.y; }

        // Update particles
        aliveCount = 0;
        for(auto& p : particles){
            if(p.life <= 0.f){ continue; }

            float dx = bhx - p.x, dy = bhy - p.y;
            float dist2 = dx*dx + dy*dy;
            float dist = std::sqrt(dist2) + 1.f;

            // Gravitational pull
            float grav = bhMass / dist2;
            grav = std::min(grav, 1200.f); // cap so particles don't teleport

            if(repulse) grav = -grav * 0.4f; // right click = push away

            p.vx += (dx/dist) * grav * dt;
            p.vy += (dy/dist) * grav * dt;

            // Slight drag (simulates gas/dust)
            p.vx *= 0.9985f;
            p.vy *= 0.9985f;

            p.x += p.vx * dt;
            p.y += p.vy * dt;

            // Consumed by black hole
            if(dist < EVENT_HORIZON){
                p.life = 0.f;
                // Spawn new particle at edge
                p.reset(bhx, bhy);
            }

            // Out of bounds respawn
            float margin = 50.f;
            if(p.x < -margin || p.x > WIDTH+margin || p.y < -margin || p.y > HEIGHT+margin){
                p.reset(bhx, bhy);
            }

            float speed = std::sqrt(p.vx*p.vx + p.vy*p.vy);
            p.color = particleColor(dist, speed);
            aliveCount++;
        }

        // Draw trails
        trailTex.draw(fadeRect, sf::RenderStates(sf::BlendNone));

        sf::VertexArray pts(sf::PrimitiveType::Points);
        for(auto& p : particles){
            if(p.life > 0.f)
                pts.append(sf::Vertex{sf::Vector2f(p.x, p.y), p.color});
        }
        trailTex.draw(pts);
        trailTex.display();

        window.clear(sf::Color::Black);
        window.draw(trailSprite);

        // Black hole visual — dark circle with glow rings
        for(int ring = 4; ring >= 0; ring--){
            float r = EVENT_HORIZON + ring * 8.f;
            uint8_t alpha = (uint8_t)(15 + ring * 8);
            sf::CircleShape glow(r, 40);
            glow.setOrigin({r, r});
            glow.setPosition({bhx, bhy});
            glow.setFillColor(sf::Color(80, 140, 255, alpha));
            window.draw(glow);
        }
        // Core
        sf::CircleShape core(EVENT_HORIZON, 40);
        core.setOrigin({EVENT_HORIZON, EVENT_HORIZON});
        core.setPosition({bhx, bhy});
        core.setFillColor(sf::Color::Black);
        core.setOutlineColor(sf::Color(120, 180, 255, 180));
        core.setOutlineThickness(2.f);
        window.draw(core);

        if(hasFont){
            hud.setString(
                "LMB: Drag Black Hole | RMB: Repulse | Space: Nova Burst | +/-: Mass | R: Reset\n"
                "Particles: " + std::to_string(aliveCount) +
                "   Mass: " + std::to_string((int)bhMass)
            );
            window.draw(hud);
        }

        window.display();
    }
    return 0;
}



