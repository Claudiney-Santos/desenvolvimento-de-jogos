#include "PathFind.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <vector>

namespace {
constexpr float SQRT2 = 1.41421356237f;

struct QueueEntry {
  float f;
  int index;
};

struct QueueCompare {
  bool operator()(const QueueEntry &a, const QueueEntry &b) const {
    return a.f > b.f;
  }
};

struct Neighbour {
  int dr;
  int dc;
  float cost;
};
} // namespace

PathFind::PathFind(const int *grid, int width, int height)
    : m_grid(grid), m_width(width), m_height(height) {}

[[nodiscard]] std::vector<Vector2D>
PathFind::findPath(const Vector2D &startWorld,
                   const Vector2D &goalWorld) const {
  const int startCol = static_cast<int>(std::floor(startWorld.x / TILE_SIZE));
  const int startRow = static_cast<int>(std::floor(startWorld.y / TILE_SIZE));
  const int goalCol = static_cast<int>(std::floor(goalWorld.x / TILE_SIZE));
  const int goalRow = static_cast<int>(std::floor(goalWorld.y / TILE_SIZE));

  const auto inRange = [this](int col, int row) {
    return col >= 0 && col < m_width && row >= 0 && row < m_height;
  };
  const auto walkable = [this](int col, int row) {
    return m_grid[static_cast<size_t>(row) * m_width + col] == EMPTY_TILE;
  };

  if (!inRange(startCol, startRow) || !inRange(goalCol, goalRow) ||
      !walkable(startCol, startRow) || !walkable(goalCol, goalRow)) {
    return {};
  }

  const int cellCount = m_width * m_height;
  const int startIndex = startRow * m_width + startCol;
  const int goalIndex = goalRow * m_width + goalCol;

  const auto heuristic = [&](int col, int row) {
    const float dx = std::abs(col - goalCol);
    const float dy = std::abs(row - goalRow);
    return std::max(dx, dy) + (SQRT2 - 1.0f) * std::min(dx, dy);
  };

  std::vector<float> gScore(static_cast<size_t>(cellCount),
                            std::numeric_limits<float>::infinity());
  std::vector<int> cameFrom(static_cast<size_t>(cellCount), -1);
  std::vector<bool> closed(static_cast<size_t>(cellCount), false);

  std::priority_queue<QueueEntry, std::vector<QueueEntry>, QueueCompare> open;
  gScore[static_cast<size_t>(startIndex)] = 0.0f;
  open.push({heuristic(startCol, startRow), startIndex});

  const Neighbour neighbours[8] = {
      {0, 1, 1.0f},  {1, 0, 1.0f},   {0, -1, 1.0f},  {-1, 0, 1.0f},
      {1, 1, SQRT2}, {1, -1, SQRT2}, {-1, 1, SQRT2}, {-1, -1, SQRT2},
  };

  while (!open.empty()) {
    const int current = open.top().index;
    open.pop();

    if (closed[static_cast<size_t>(current)]) {
      continue;
    }
    if (current == goalIndex) {
      break;
    }
    closed[static_cast<size_t>(current)] = true;

    const int col = current % m_width;
    const int row = current / m_width;

    for (const Neighbour &n : neighbours) {
      const int nextCol = col + n.dc;
      const int nextRow = row + n.dr;
      if (!inRange(nextCol, nextRow) || !walkable(nextCol, nextRow)) {
        continue;
      }
      if (n.dr != 0 && n.dc != 0) {
        if (!walkable(row + n.dr, col) || !walkable(row, col + n.dc)) {
          continue;
        }
      }

      const int next = nextRow * m_width + nextCol;
      if (closed[static_cast<size_t>(next)]) {
        continue;
      }

      const float tentative = gScore[static_cast<size_t>(current)] + n.cost;
      if (tentative < gScore[static_cast<size_t>(next)]) {
        cameFrom[static_cast<size_t>(next)] = current;
        gScore[static_cast<size_t>(next)] = tentative;
        open.push({tentative + heuristic(nextCol, nextRow), next});
      }
    }
  }

  if (cameFrom[static_cast<size_t>(goalIndex)] == -1 &&
      goalIndex != startIndex) {
    return {};
  }

  std::vector<int> cells;
  for (int cur = goalIndex; cur != -1;
       cur = cameFrom[static_cast<size_t>(cur)]) {
    cells.push_back(cur);
    if (cur == startIndex) {
      break;
    }
  }
  std::reverse(cells.begin(), cells.end());

  std::vector<Vector2D> path;
  path.reserve(cells.size());
  for (int index : cells) {
    const int col = index % m_width;
    const int row = index / m_width;
    path.push_back(
        Vector2D((col + 0.5f) * TILE_SIZE, (row + 0.5f) * TILE_SIZE));
  }
  return path;
}
