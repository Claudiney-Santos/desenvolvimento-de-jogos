#include "TileMap.hpp"
#include "Dungeon2D.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace {
bool parseInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (end == s.c_str() || *end != '\0') return false;
    out = static_cast<int>(v);
    return true;
}
} // namespace

void TileMap::loadFromDungeon(const Dungeon2D& dungeon) {
    m_width = dungeon.width;
    m_height = dungeon.height;
    m_cells.assign(static_cast<size_t>(m_width) * m_height, WALL_TILE);

    std::vector<const BSPNode*> stack;
    if (dungeon.root) stack.push_back(dungeon.root.get());
    while (!stack.empty()) {
        const BSPNode* node = stack.back();
        stack.pop_back();
        if (node->isLeaf()) {
            const Rect& room = node->room;
            for (int row = room.top; row <= room.bottom; ++row) {
                for (int col = room.left; col <= room.right; ++col) {
                    if (col >= 0 && col < m_width && row >= 0 && row < m_height) {
                        m_cells[static_cast<size_t>(row) * m_width + col] = EMPTY_TILE;
                    }
                }
            }
        } else {
            if (node->right) stack.push_back(node->right.get());
            if (node->left) stack.push_back(node->left.get());
        }
    }

    for (const Corridor& c : dungeon.corridors) {
        for (int row = c.top; row <= c.bottom; ++row) {
            for (int col = c.left; col <= c.right; ++col) {
                if (col >= 0 && col < m_width && row >= 0 && row < m_height) {
                    m_cells[static_cast<size_t>(row) * m_width + col] = EMPTY_TILE;
                }
            }
        }
    }
}

bool TileMap::saveCSV(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            if (col > 0) out << ',';
            out << m_cells[static_cast<size_t>(row) * m_width + col];
        }
        out << '\n';
    }
    return out.good();
}

bool TileMap::loadCSV(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    std::vector<int> cells;
    int w = 0;
    int h = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<int> row;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            int value = 0;
            if (!parseInt(token, value)) return false;
            row.push_back(value);
        }
        if (h == 0) {
            w = static_cast<int>(row.size());
        } else if (static_cast<int>(row.size()) != w) {
            return false;
        }
        cells.insert(cells.end(), row.begin(), row.end());
        ++h;
    }
    if (h == 0) return false;

    m_width = w;
    m_height = h;
    m_cells = std::move(cells);
    return true;
}

int TileMap::width() const noexcept {
    return m_width;
}

int TileMap::height() const noexcept {
    return m_height;
}

int TileMap::at(int col, int row) const {
    assert(col >= 0 && col < m_width && row >= 0 && row < m_height);
    return m_cells[static_cast<size_t>(row) * m_width + col];
}

bool TileMap::isWall(int col, int row) const {
    return at(col, row) == WALL_TILE;
}

const int* TileMap::data() const noexcept {
    return m_cells.data();
}

void TileMap::toIndices(const Vector2D& world, int& col, int& row) const noexcept {
    col = static_cast<int>(std::floor(world.x / TILE_SIZE));
    row = static_cast<int>(std::floor(world.y / TILE_SIZE));
}

Vector2D TileMap::toWorld(int col, int row) const noexcept {
    return Vector2D((col + 0.5f) * TILE_SIZE, (row + 0.5f) * TILE_SIZE);
}

void TileMap::randomEmptyCell(int& col, int& row) const {
    std::vector<int> emptyIndices;
    emptyIndices.reserve(m_cells.size());
    for (size_t i = 0; i < m_cells.size(); ++i) {
        if (m_cells[i] == EMPTY_TILE) emptyIndices.push_back(static_cast<int>(i));
    }
    assert(!emptyIndices.empty());

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, emptyIndices.size() - 1);
    size_t index = static_cast<size_t>(emptyIndices[dist(rng)]);
    col = static_cast<int>(index % static_cast<size_t>(m_width));
    row = static_cast<int>(index / static_cast<size_t>(m_width));
}
