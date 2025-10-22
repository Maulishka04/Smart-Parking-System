# Smart Parking System - Performance and Complexity

## Complexity Summary
- N = total spots = FLOORS * SPOTS_PER_FLOOR = 100
- Entry allocation: O(N) worst-case (linear scan). With N=100, negligible.
- Search by license: O(N) linear scan. For simplicity and small N, acceptable. In larger systems, maintain a hash map from license -> (floor, spot) for O(1).
- Exit: O(1) + search O(N)
- Reports:
  - Occupancy: O(N)
  - Revenue: O(T) where T = transactions count
  - Peak Entry Hour: O(T + N)

## Memory Usage
- Each parked vehicle allocates one record (C: malloc; C++: unique_ptr). With at most 100 vehicles, memory usage is trivial.

## I/O Considerations
- Parking state is fully rewritten on each change; file is small (<10KB). Transactions are append-only.

## Potential Optimizations
- Maintain an index (unordered_map in C++; a hash table in C) from license to spot to reduce search to O(1).
- Use a min-heap keyed by (floor,spot) for nearest-spot allocation if layout changes.
- Batch state writes or use journaling for higher throughput.

## Concurrency
- Current design is single-process, single-threaded CLI. For concurrent kiosks, guard files (advisory locks) and centralize state (database).

