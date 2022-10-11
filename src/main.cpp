#include <iostream>
#include <cmath>
#include <optional>
#include <vector>
#include <uuid.h>
#include <unordered_set>
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
    Point(int X_, int Y_, int Z_) : X{X_}, Y{Y_}, Z{Z_} {

    }

    Point() : Point(0, 0, 0) {

    }

    bool operator==(const Point& Other) const {
        return this->X == Other.X && this->Y == Other.Y && this->Z == Other.Z;
    }

    int X;
    int Y;
    int Z;
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

struct Zone {
    static constexpr const float Size = 500.0F; // The Area Size of a Zone in Meters. Needed for the Movement between Zones

    explicit Zone(Point ZoneCoordinate)
            : Coordinate{ZoneCoordinate}, Entities{} {

    }

    Point Coordinate; // Zone-Space Coordinate. For example Zone A (0, 0) and Zone B(1, 0)
    std::unordered_set<EntityId> Entities; // All the entities in a specific Zone.
};

using ZonePointMap = std::unordered_map<Point, Zone>;

enum class ComponentType {
    Health,
    Movement
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

struct MovementComponent : BaseComponent {
    explicit MovementComponent(ComponentId Identifier)
            : BaseComponent{Identifier, ComponentType::Movement}, NewLocation{} {

    }

    Vec3 NewLocation;
};

using ComponentIdMap = std::unordered_map<ComponentId, BaseComponent*>;
using ComponentTypeMap = std::unordered_map<ComponentType, BaseComponent*>;

struct Entity {
    explicit Entity(EntityId Identifier)
            : Id{Identifier}, Components{}, ZoneCoordinate{}, Location{} {

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
    Point ZoneCoordinate; // Which Zone they are in.
    Vec3 Location; // TODO: Location within a Zone space. Move to a Transform Component?
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

    static bool AddEntityToZone(ZonePointMap& Zones, Entity* EntityToAdd, Point ZoneCoordinateToAdd) {
        if (EntityToAdd == nullptr) {
            printf("Invalid entity to add to zone.\n");
            return false;
        }

        Zone* ZoneToAdd = GetZoneFromCoord(Zones, ZoneCoordinateToAdd);

        if (ZoneToAdd == nullptr) {
            printf("Could not find the Zone to spawn in.\n");
            return false;
        }

        ZoneToAdd->Entities.emplace(EntityToAdd->Id);
        EntityToAdd->ZoneCoordinate = ZoneToAdd->Coordinate;

        return true;
    }

    static bool RemoveEntityFromZone(ZonePointMap& Zones, Entity* EntityToRemove) {
        if (EntityToRemove == nullptr) {
            printf("Invalid entity to remove from the zone.\n");
            return false;
        }

        Zone* ZoneToRemove = GetZoneFromCoord(Zones, EntityToRemove->ZoneCoordinate);

        if (ZoneToRemove == nullptr) {
            printf("Very bad bug man. Every Entities needs a zone.\n");
            // TODO: Change this to assert.
            return false;
        }

        ZoneToRemove->Entities.erase(EntityToRemove->Id);
        EntityToRemove->ZoneCoordinate = Point{};

        return true;
    }

    static bool UpdateEntityZone(ZonePointMap& Zones, Entity* EntityToUpdate) {
        if (EntityToUpdate == nullptr) {
            printf("Invalid entity to update the zone.\n");
            return false;
        }

        Point NewZoneCoord = EntityToUpdate->ZoneCoordinate;
        Vec3 CurrentLocation = EntityToUpdate->Location;
        Vec3 NewLocation = CurrentLocation;

        if (CurrentLocation.X > Zone::Size) {
            NewLocation.X = CurrentLocation.X - Zone::Size;
            ++NewZoneCoord.X;
        } else if (CurrentLocation.X < -Zone::Size) {
            NewLocation.X = Zone::Size - (std::abs(CurrentLocation.X) - Zone::Size);
            --NewZoneCoord.X;
        } else if (CurrentLocation.Y > Zone::Size) {
            NewLocation.Y = CurrentLocation.Y - Zone::Size;
            ++NewZoneCoord.Y;
        } else if (CurrentLocation.Y < -Zone::Size) {
            NewLocation.Y = Zone::Size - (std::abs(CurrentLocation.Y) - Zone::Size);
            --NewZoneCoord.Y;
        }

        EntityToUpdate->Location = NewLocation;

        if (NewZoneCoord != EntityToUpdate->ZoneCoordinate) {
            RemoveEntityFromZone(Zones, EntityToUpdate);
            AddEntityToZone(Zones, EntityToUpdate, NewZoneCoord);
        }

        return true;
    }
}

namespace MovementInput {
    static void MoveForward(Entity* EntityToMove, float Value) {
        MovementComponent* Movement = EntityToMove->GetComponent<MovementComponent>(ComponentType::Movement);

        if (!Movement) {
            printf("Bruh!\n");
            return;
        }

        Movement->NewLocation.Y = Value;
    }

    static void MoveRight(Entity* EntityToMove, float Value) {
        MovementComponent* Movement = EntityToMove->GetComponent<MovementComponent>(ComponentType::Movement);

        if (!Movement) {
            printf("Bruh!\n");
            return;
        }

        Movement->NewLocation.X = Value;
    }

    static void MoveUp(Entity* EntityToMove, float Value) {
        MovementComponent* Movement = EntityToMove->GetComponent<MovementComponent>(ComponentType::Movement);

        if (!Movement) {
            printf("Bruh!\n");
            return;
        }

        Movement->NewLocation.Z = Value;
    }
}

namespace EntityUpdateManager {
    static void DoEntityMovement(Entity* EntityToMove, ZonePointMap& Zones) {
        MovementComponent* Movement = EntityToMove->GetComponent<MovementComponent>(ComponentType::Movement);

        if (!Movement) {
            return;
        }

        // TODO: Move this to physics system
        EntityToMove->Location += Movement->NewLocation;
        Movement->NewLocation = Vec3{};
        ZoneManager::UpdateEntityZone(Zones, EntityToMove);
    }

    static void UpdateZone(Zone* ZoneToUpdate, ZonePointMap& Zones, EntityIdMap& Entities) {
        auto EntitiesSnapshot = ZoneToUpdate->Entities;
        for (EntityId EntityInZoneId: EntitiesSnapshot) {
            DoEntityMovement(&Entities.at(EntityInZoneId), Zones);
        }
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
        ZoneManager::AddEntityToZone(Zones, SpawnedEntity, ZoneCoordinate);

        return SpawnedEntity;
    }

    void DestroyEntity(Entity* EntityToDestroy) {
        ZoneManager::RemoveEntityFromZone(Zones, EntityToDestroy);
        Entities.Destroy(EntityToDestroy->Id);
    }

    template<typename T>
    T* CreateComponent(Entity* EntityToAttachTo) {
        T* NewComponent = Components.CreateComponent<T>(UuidRng());
        EntityStorage::AttachComponent(EntityToAttachTo, NewComponent);

        return NewComponent;
    }

    void Update(Point ZoneToUpdateCoord) {
        EntityUpdateManager::UpdateZone(ZoneManager::GetZoneFromCoord(Zones, ZoneToUpdateCoord), Zones, Entities.GetEntities());
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
    Entity* MainCharacter = CurrentWorld.SpawnEntity(Point{0, 0, 0});
    MovementComponent* Movement = CurrentWorld.CreateComponent<MovementComponent>(MainCharacter);

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
            float Speed = 350.0F;

            if (Input == "forward") {
                MovementInput::MoveForward(MainCharacter, Speed);
                std::cout << "You walked forward.\n";
            } else if (Input == "back") {
                MovementInput::MoveForward(MainCharacter, -Speed);
                std::cout << "You walked back.\n";
            } else if (Input == "left") {
                MovementInput::MoveRight(MainCharacter, -Speed);
                std::cout << "You walked left.\n";
            } else if (Input == "right") {
                MovementInput::MoveRight(MainCharacter, Speed);
                std::cout << "You walked right.\n";
            } else if (Input == "up") {
                MovementInput::MoveUp(MainCharacter, Speed);
                std::cout << "You walked up.\n";
            } else if (Input == "down") {
                MovementInput::MoveUp(MainCharacter, -Speed);
                std::cout << "You walked down.\n";
            }
        }

        CurrentWorld.Update(MainCharacter->ZoneCoordinate);

        std::cout << "Player Location is " << MainCharacter->Location << '\n';
        std::cout << "Player Zone is " << MainCharacter->ZoneCoordinate << '\n';
    }

    while (!Input.empty());

    return 0;
}
