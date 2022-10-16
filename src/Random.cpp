#include "Random.h"


Random::Random() : Rng{}, UuidRng{uuids::uuid_random_generator(Rng)} {
}
