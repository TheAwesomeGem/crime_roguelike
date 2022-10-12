#include <iostream>
#include <cmath>
#include <optional>
#include <vector>
#include <uuid.h>
#include <unordered_set>
#include <memory>
#include <deque>
#include "Delegates.h"


#define PI                  3.141592F
#define DEG2RAD             (PI / 180.0F)
#define RAD2DEG             (180.0F / PI)


#define CLAMP(value, min, max)           (value >= min ? (value <= max ? value : max) : min)

inline void hash_combine(std::size_t& seed) {
}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

template<typename T>
inline bool FracEqual(T A, T B) {
    return std::abs(A - B) < std::numeric_limits<T>::epsilon();
}


// TODO: Add Zone System. Make a new coordinate space called a Zone. It's a subsection of the World space that encompass the game world.

using EntityId = uuids::uuid;
using ComponentId = uuids::uuid;

struct Point {
    Point(int32_t X_, int32_t Y_, int32_t Z_) : X{X_}, Y{Y_}, Z{Z_} {

    }

    Point() : Point(0, 0, 0) {

    }

    bool operator==(const Point& Other) const {
        return this->X == Other.X && this->Y == Other.Y && this->Z == Other.Z;
    }

    int32_t X;
    int32_t Y;
    int32_t Z;
};

struct Vec3 {
    Vec3(float X_, float Y_, float Z_) : X{X_}, Y{Y_}, Z{Z_} {

    }

    Vec3() : Vec3(0.0F, 0.0F, 0.0F) {

    }

    Vec3 operator*(float Scalar) const {
        return Vec3{X * Scalar, Y * Scalar, Z * Scalar};
    }

    Vec3 operator/(float Scalar) const {
        return *this * (1 / Scalar);
    }

    Vec3 operator-() const {
        return *this * -1.0F;
    }

    Vec3 operator+(const Vec3& Other) const {
        Vec3 Coord{X + Other.X, Y + Other.Y, Z + Other.Z};

        return Coord;
    }

    Vec3 operator-(const Vec3& Other) const {
        return *this + (-Other);
    }

    Vec3& operator+=(const Vec3& Other) {
        *this = *this + Other;

        return *this;
    }

    bool operator==(const Vec3& Other) const {
        return FracEqual(X, Other.X) && FracEqual(Y, Other.Y) && FracEqual(Z, Other.Z);
    }

    float Dot(const Vec3& Other) const // The length of A and the length of B and the cosine between A and B
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    float LengthSq() const {
        return Dot(*this);
    }

    float Length() const {
        return sqrtf(LengthSq());
    }

    Vec3 Normalized() const {
        return *this / Length();
    }

    Vec3 Displacement(const Vec3& Other) const {
        return Other - *this;
    }

    float DistanceSq(const Vec3& Other) const {
        return Displacement(Other).LengthSq();
    }

    float Distance(const Vec3& Other) const {
        return sqrtf(DistanceSq(Other));
    }

    Vec3 Direction(const Vec3& Other) const {
        return Displacement(Other).Normalized();
    }

    float AngleBetween(const Vec3& Other) const {
        return acosf(Dot(Other)) * RAD2DEG;
    }

    float X;
    float Y;
    float Z;
};

namespace std {
    template<>
    struct hash<Vec3> {
        inline size_t operator()(const Vec3& Vec) const {
            size_t value = 0;
            hash_combine(value, Vec.X, Vec.Y);

            return value;
        }
    };

    template<>
    struct hash<Point> {
        inline size_t operator()(const Point& ThePoint) const {
            size_t value = 0;
            hash_combine(value, ThePoint.X, ThePoint.Y);

            return value;
        }
    };
}

enum class ZoneComponentType {
    Police
};

struct ZoneComponent {
    explicit ZoneComponent(ZoneComponentType ComponentType)
            : Type{ComponentType} {

    }

    ZoneComponentType Type;
};


using ZoneComponentTypeMap = std::unordered_map<ZoneComponentType, std::unique_ptr<ZoneComponent>>;

struct Zone {
    static constexpr const uint8_t TURNS_TO_REGENERATE = 4;

    explicit Zone(Point ZoneCoordinate)
            : Coordinate{ZoneCoordinate}, TurnsSinceLastCreated{-1}, NPCEntities{}, Components{} {

    }

    template<typename T>
    void AddComponent(ZoneComponent* ComponentToAdd) {
        if (Components.find(ComponentToAdd->Type) != Components.end()) {
            printf("Component already exists.\n");
            return;
        }

        Components.emplace(ComponentToAdd->Type, std::unique_ptr<ZoneComponent>{ComponentToAdd});
    }

    template<typename T>
    void RemoveComponent(ZoneComponentType ComponentTypeToRemove) {
        if (Components.find(ComponentTypeToRemove) == Components.end()) {
            printf("Component does not exists already.\n");
            return;
        }

        Components.erase(ComponentTypeToRemove);
    }

    template<typename T>
    T* GetComponent(ZoneComponentType Type) {
        auto ComponentIt = Components.find(Type);

        if (ComponentIt == Components.end()) {
            return nullptr;
        }

        return dynamic_cast<T*>(ComponentIt->second.get());
    }

    Point Coordinate; // Zone-Space Coordinate. For example Zone A (0, 0) and Zone B(1, 0)
    int16_t TurnsSinceLastCreated;
    std::vector<EntityId> NPCEntities; // TODO: Should group them between civilian and police
    // TODO: Removed Playable Entity Cache from Zone because there wouldn't be a lot of Playable Entities anyway. Maybe need to revisit later?

private:
    ZoneComponentTypeMap Components; // All the components that extend the state of the Zone.
};

using ZonePointMap = std::unordered_map<Point, Zone>;

enum class ComponentType {
    Health
};

struct BaseComponent {
    MulticastDelegate<ComponentId> DestroyEvent;

    BaseComponent(ComponentId Identifier, ComponentType ComponentType)
            : Id{Identifier}, Type{ComponentType} {

    }

    virtual void Destroy() {
        DestroyEvent.Broadcast(Id);
    }

    ComponentId Id;
    ComponentType Type;
};

struct HealthComponent : BaseComponent {
    MulticastDelegate<> DeathEvent;

    explicit HealthComponent(ComponentId Identifier)
            : BaseComponent{Identifier, ComponentType::Health}, CurrentHealth{0.0F}, MaxHealth{0.0F} {

    }


    bool IsAlive() const {
        return CurrentHealth > 0.0F;
    }

    bool IsDead() const {
        return !IsAlive();
    }

    void SetHealth(float Health) {
        float NewHealth = CLAMP(Health, -std::numeric_limits<float>::max(), MaxHealth);

        CurrentHealth = NewHealth;

        if (NewHealth <= 0.0F) {
            DeathEvent.Broadcast();
        }
    }

    void SetMaxHealth(float Health) {
        float NewMaxHealth = CLAMP(Health, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

        MaxHealth = NewMaxHealth;
    }

    float Health() const {
        return CurrentHealth;
    }

    float HealthMax() const {
        return MaxHealth;
    }

private:
    float CurrentHealth;
    float MaxHealth;
};

using ComponentIdMap = std::unordered_map<ComponentId, BaseComponent*>;
using ComponentTypeMap = std::unordered_map<ComponentType, BaseComponent*>;

struct Entity {
    explicit Entity(EntityId Identifier)
            : Id{Identifier}, Components{}, ZoneCoordinate{} {

    }

    template<typename T>
    T* GetComponent(ComponentType Type) {
        auto ComponentIt = Components.find(Type);

        if (ComponentIt == Components.end()) {
            return nullptr;
        }

        return dynamic_cast<T*>(ComponentIt->second);
    }

public:
    EntityId Id;
    ComponentTypeMap Components;
    Point ZoneCoordinate;
};

class ComponentStorage {
public:
    template<typename T>
    T* CreateComponent(ComponentId IdToAdd) {
        T* NewComponent = new T{IdToAdd};
        AddComponent(NewComponent);

        return NewComponent;
    }

    bool Destroy(ComponentId IdToDestroy) {
        auto ComponentIt = Components.find(IdToDestroy);

        if (ComponentIt != Components.end()) {
            BaseComponent* ComponentToDestroy = ComponentIt->second;
            ComponentToDestroy->Id = uuids::uuid{};
            delete ComponentToDestroy;
            Components.erase(ComponentIt);

            return true;
        }

        return false;
    }

private:
    void AddComponent(BaseComponent* ComponentToAdd) {
        auto ComponentIt = Components.emplace(ComponentToAdd->Id, ComponentToAdd);
        ComponentToAdd->DestroyEvent.AddRaw(this, &ComponentStorage::OnDestroy);
    }

    void OnDestroy(ComponentId Id) {
        Destroy(Id);
    }

private:
    ComponentIdMap Components;
};

using EntityIdMap = std::unordered_map<EntityId, Entity>;

class EntityStorage {
public:
    Entity* Create(EntityId Id) {
        auto EntityIt = Entities.emplace(Id, Id);

        return &EntityIt.first->second;
    }

    bool Destroy(EntityId Id) {
        auto EntityIt = Entities.find(Id);

        if (EntityIt != Entities.end()) {
            Entity& EntityToDestroy = EntityIt->second;
            EntityToDestroy.Id = uuids::uuid{};

            for (auto& [_, ComponentToDestroy]: EntityToDestroy.Components) {
                RemoveComponent(&EntityToDestroy, ComponentToDestroy);
            }

            Entities.erase(EntityIt);

            return true;
        }

        return false;
    }

    Entity* GetEntityFromId(EntityId IdToGet) {
        auto EntityIt = Entities.find(IdToGet);

        if (EntityIt == Entities.end()) {
            return nullptr;
        }

        return &EntityIt->second;
    }

    EntityIdMap& GetEntities() {
        return Entities;
    }

    static void AttachComponent(Entity* Entity, BaseComponent* ComponentToAttach) {
        Entity->Components.emplace(ComponentToAttach->Type, ComponentToAttach);
    }

    static bool RemoveComponent(Entity* Entity, BaseComponent* ComponentToRemove) {
        ComponentToRemove->Destroy();
        Entity->Components.erase(ComponentToRemove->Type);
    }

private:
    EntityIdMap Entities;
};

namespace ZoneManager {
    static Zone* GetZoneFromCoord(ZonePointMap& Zones, Point ZoneCoordinate) {
        auto ZoneIt = Zones.find(ZoneCoordinate);

        if (ZoneIt == Zones.end()) {
            return nullptr;
        }

        return &ZoneIt->second;
    }
}

class World {
public:
    static constexpr const uint16_t ZONES_ROWS = 5;
    static constexpr const uint16_t ZONES_COLS = 5;
    static constexpr const uint16_t ZONES_DEPTH = 3;

public:
    World()
            : Rng{}, UuidRng{uuids::uuid_random_generator(Rng)} {
        GenerateEmptyZones();
    }

    Entity* SpawnEntity(Point ZoneCoordinate) {
        Entity* SpawnedEntity = Entities.Create(UuidRng());
        SpawnedEntity->ZoneCoordinate = ZoneCoordinate;

        return SpawnedEntity;
    }

    void DestroyEntity(Entity* EntityToDestroy) {
        Entities.Destroy(EntityToDestroy->Id);
    }

    Entity* GetEntity(EntityId IdToGet) {
        return Entities.GetEntityFromId(IdToGet);
    }

    template<typename T>
    T* CreateComponent(Entity* EntityToAttachTo) {
        T* NewComponent = Components.CreateComponent<T>(UuidRng());
        EntityStorage::AttachComponent(EntityToAttachTo, NewComponent);

        return NewComponent;
    }

public:
    ComponentStorage Components;
    EntityStorage Entities;
    ZonePointMap Zones;

private:
    void GenerateEmptyZones() {
        for (uint16_t Z = 0; Z < ZONES_DEPTH; ++Z) {
            for (uint16_t Y = 0; Y < ZONES_COLS; ++Y) {
                for (uint16_t X = 0; X < ZONES_ROWS; ++X) {
                    Point ZoneCoordinate = Point{X, Y, Z};
                    Zones.emplace(ZoneCoordinate, Zone{ZoneCoordinate});
                }
            }
        }
    }

private:
    std::mt19937 Rng;
    uuids::uuid_random_generator UuidRng;
};

struct Command {
    explicit Command(World* World) : CurrentWorld{World} {

    }

    virtual ~Command() = default;

    virtual void Execute() = 0;

private:
    World* CurrentWorld;
};

class Controller {
public:
    Controller() : MainControlledEntity{nullptr}, ControlledEntities{} {

    }

    void Possess(Entity* MainEntity) {
        ControlledEntities.push_back(MainEntity->Id);
        MainControlledEntity = MainEntity;
    }

    void AddCommand(Command* CommandToAdd) {
        Commands.push_back(std::unique_ptr<Command>{CommandToAdd});
    }

    Command* GetCommand() const {
        return Commands.front().get();
    }

    void PopCommand() {
        Commands.pop_front();
    }

    bool HasCommands() const {
        return !Commands.empty();
    }

private:
    Entity* MainControlledEntity; // TODO: Perhaps this should be an ID instead of a Pointer? But then again, game is over for AI/Player after they lose the ControlledEntity anyway.
    std::deque<std::unique_ptr<Command>> Commands;

public:
    std::vector<EntityId> ControlledEntities;
};

struct EventData {
    EventData()
            : ZoneCoordinate{}, Instigator{nullptr}, Target{nullptr} {

    }

    Point ZoneCoordinate;
    // EventType Type;
    Entity* Instigator;
    Entity* Target;
};

struct History {
    std::vector<EventData> Events;
};

namespace CommandProcessor {
    static void ProcessCommand(Controller* Controller) {
        while (Controller->HasCommands()) {
            Controller->GetCommand()->Execute();
            Controller->PopCommand();
        }
    }
}

namespace ZoneUpdate {
    static void UpdateZone(World* CurrentWorld, Point ZoneCoordinate) {
        Zone* ZoneToUpdate = ZoneManager::GetZoneFromCoord(CurrentWorld->Zones, ZoneCoordinate);

        if (ZoneToUpdate == nullptr) {
            printf("HUGE PROBLEM HERE FOLKS!\n");

            return;
        }

        if (ZoneToUpdate->TurnsSinceLastCreated == -1 || ZoneToUpdate->TurnsSinceLastCreated >= Zone::TURNS_TO_REGENERATE) {
            // TODO: Regenerate zone.
        }

        // TODO: Updates each component that needs to be updated and use the NPC entities to generate commands against the playable entities.
    }
}

namespace UpdateManager {
    static void Update(World* CurrentWorld, Controller* Controller) {
        CommandProcessor::ProcessCommand(Controller);

        for (EntityId ControlledEntityId: Controller->ControlledEntities) {
            Entity* ControlledEntity = CurrentWorld->GetEntity(ControlledEntityId);
            if (ControlledEntity == nullptr) {
                continue;
            }

            ZoneUpdate::UpdateZone(CurrentWorld, ControlledEntity->ZoneCoordinate);
        }
    }
}

std::ostream& operator<<(std::ostream& stream, Vec3 Location) {
    stream << "[" << Location.X << ", " << Location.Y << "]";

    return stream;
}

std::ostream& operator<<(std::ostream& stream, Point Coordinate) {
    stream << "(" << Coordinate.X << ", " << Coordinate.Y << ")";

    return stream;
}

int main() {
    World CurrentWorld;
    Controller PlayerController{};
    PlayerController.Possess(CurrentWorld.SpawnEntity(Point{0, 0, 0}));


    // TODO: Add turn system and for now, it will be only Player. Add all the commands a Player makes before they finish the turn and process it in an update loop to
    // TODO: generate states for the next turn. Rinse and repeat.

    // TODO: Maybe instead of using events, anytime an entity moves to a zone, we generate state to that zone. For example, we will generate other entities randomly.
    // TODO: Whenever an entity leaves the zone, the state gets cleared. We will need a distinction between player/AI controlled entities and generated NPC entities.
    // TODO: NPC entities should not be allowed to travel between zones and they are stuck on the zone they are generated in and gets destroyed once playable entity leaves the zone.
    // TODO: Now after the temporary state of a zone gets generated, player can assign commands to the playable entity based on the state of the zone.
    // TODO: There will be permanent state of the zone and that can be stored inside a component. Things such as police population and civilian population and the type of zone.

    // TODO: ControlledEntities = Playable Entities
    // TODO: !ControlledEntities = NPC Entities

    UpdateManager::Update(&CurrentWorld, &PlayerController); // Initial Update to Regenerate all the zones that a Playable Entity is at.

    std::string Input;
    do {
        std::cout << "Type your input: \n";
        std::getline(std::cin, Input);

        if (Input.empty()) {
            continue;
        }

        std::transform(Input.begin(), Input.end(), Input.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (Input == "forward" || Input == "back" || Input == "left" || Input == "right") {
            // TODO: Add commands to player controller here.
        }

        if (Input == "end") {
            // TODO: Player has ended the turn, let TurnManager dictate who takes their turn next.
        }

        UpdateManager::Update(&CurrentWorld, &PlayerController); // TODO: This will be a list of Controllers after adding AI.
    }

    while (!Input.empty());

    return 0;
}
