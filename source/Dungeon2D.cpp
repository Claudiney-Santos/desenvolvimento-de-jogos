#include "Dungeon2D.hpp"
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

namespace {
constexpr int MIN_ROOM = 5;
constexpr int CORRIDOR_W = 3;
constexpr int ROOM_MARGIN = 1;

int randInt(std::mt19937& rng, int lo, int hi) {
    return std::uniform_int_distribution<int>(lo, hi)(rng);
}

int regionWidth(const Rect& r) {
    return r.right - r.left + 1;
}

int regionHeight(const Rect& r) {
    return r.bottom - r.top + 1;
}

bool canSplit(const Rect& r) {
    const int minHalf = MIN_ROOM + 2 * ROOM_MARGIN;
    return regionWidth(r) >= 2 * minHalf || regionHeight(r) >= 2 * minHalf;
}

Rect carveRoom(const Rect& region, std::mt19937& rng) {
    int w = regionWidth(region);
    int h = regionHeight(region);
    int maxRoomW = std::max(MIN_ROOM, w - 2 * ROOM_MARGIN);
    int maxRoomH = std::max(MIN_ROOM, h - 2 * ROOM_MARGIN);
    int roomW = (maxRoomW == MIN_ROOM) ? std::min(MIN_ROOM, w)
                                       : randInt(rng, MIN_ROOM, maxRoomW);
    int roomH = (maxRoomH == MIN_ROOM) ? std::min(MIN_ROOM, h)
                                       : randInt(rng, MIN_ROOM, maxRoomH);
    int minX = region.left + ROOM_MARGIN;
    int maxX = region.right - roomW + 1 - ROOM_MARGIN;
    int minY = region.top + ROOM_MARGIN;
    int maxY = region.bottom - roomH + 1 - ROOM_MARGIN;
    int x = (minX <= maxX) ? randInt(rng, minX, maxX) : region.left;
    int y = (minY <= maxY) ? randInt(rng, minY, maxY) : region.top;
    return Rect{x, y, x + roomW - 1, y + roomH - 1};
}

std::vector<Rect> collectRooms(const BSPNode* node) {
    std::vector<Rect> rooms;
    std::vector<const BSPNode*> stack{node};
    while (!stack.empty()) {
        const BSPNode* cur = stack.back();
        stack.pop_back();
        if (cur->isLeaf()) {
            rooms.push_back(cur->room);
        } else {
            if (cur->right) stack.push_back(cur->right.get());
            if (cur->left) stack.push_back(cur->left.get());
        }
    }
    return rooms;
}

void connect(BSPNode* node, std::mt19937& rng, std::vector<Corridor>& corridors) {
    std::vector<Rect> leftRooms = collectRooms(node->left.get());
    std::vector<Rect> rightRooms = collectRooms(node->right.get());
    const Rect& a = leftRooms[randInt(rng, 0, static_cast<int>(leftRooms.size()) - 1)];
    const Rect& b = rightRooms[randInt(rng, 0, static_cast<int>(rightRooms.size()) - 1)];
    int x1 = randInt(rng, a.left, a.right);
    int y1 = randInt(rng, a.top, a.bottom);
    int x2 = randInt(rng, b.left, b.right);
    int y2 = randInt(rng, b.top, b.bottom);
    const int half = CORRIDOR_W / 2;
    if (randInt(rng, 0, 1) == 0) {
        corridors.push_back(Rect{std::min(x1, x2), y1 - half, std::max(x1, x2), y1 + half});
        corridors.push_back(Rect{x2 - half, std::min(y1, y2), x2 + half, std::max(y1, y2)});
    } else {
        corridors.push_back(Rect{x1 - half, std::min(y1, y2), x1 + half, std::max(y1, y2)});
        corridors.push_back(Rect{std::min(x1, x2), y2 - half, std::max(x1, x2), y2 + half});
    }
}

void buildNode(BSPNode* node, std::mt19937& rng, std::vector<Corridor>& corridors) {
    if (!canSplit(node->region) ||
        std::uniform_real_distribution<double>(0.0, 1.0)(rng) > 0.85) {
        node->room = carveRoom(node->region, rng);
        return;
    }
    const Rect& r = node->region;
    const int minHalf = MIN_ROOM + 2 * ROOM_MARGIN;
    bool canVertical = regionWidth(r) >= 2 * minHalf;
    bool canHorizontal = regionHeight(r) >= 2 * minHalf;
    bool vertical = canVertical && canHorizontal
                        ? (regionWidth(r) != regionHeight(r)
                               ? regionWidth(r) > regionHeight(r)
                               : randInt(rng, 0, 1) == 0)
                        : canVertical;
    node->left = std::make_unique<BSPNode>();
    node->right = std::make_unique<BSPNode>();
    if (vertical) {
        int mid = randInt(rng, r.left + minHalf, r.right - minHalf + 1);
        node->left->region = Rect{r.left, r.top, mid - 1, r.bottom};
        node->right->region = Rect{mid, r.top, r.right, r.bottom};
    } else {
        int mid = randInt(rng, r.top + minHalf, r.bottom - minHalf + 1);
        node->left->region = Rect{r.left, r.top, r.right, mid - 1};
        node->right->region = Rect{r.left, mid, r.right, r.bottom};
    }
    buildNode(node->left.get(), rng, corridors);
    buildNode(node->right.get(), rng, corridors);
    connect(node, rng, corridors);
}
} // namespace

Dungeon2D::Dungeon2D(int widthTiles, int heightTiles)
    : width(widthTiles), height(heightTiles) {
    assert(widthTiles > 0 && heightTiles > 0);
}

void Dungeon2D::generate(unsigned seed) {
    std::mt19937 rng(seed);
    corridors.clear();
    root = std::make_unique<BSPNode>();
    root->region = Rect{0, 0, width - 1, height - 1};
    buildNode(root.get(), rng, corridors);
}
