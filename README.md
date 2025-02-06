# Horse-Riding-Tracking-System
Overview:
Designed and implemented a high‐performance system to manage horse riding teams, track jockeys’ personal and team records, and handle dynamic merges of teams in near‐constant time. Used custom Union–Find for team merges, multiple hash tables for O(1) lookups, and rigorous memory‐management checks.

Key Achievements & Technical Highlights:

Union–Find (Disjoint Set): Developed a specialized structure to efficiently merge teams (amortized O(1) “find” operations, path compression).
Multiple Hash Tables:
Teams by ID,
Jockeys by ID,
Teams by total record,
Banned IDs (prevent re‐adding lost team IDs).

Achieved O(1) expected insertion/lookup via separate chaining.

Complexity Guarantees:
Each major operation (add_team, add_jockey, update_match, merge_teams, unite_by_record, get_*_record) runs in O(1) average time.

The entire data structure uses O(n + m) space, where n is the number of jockeys and m is the total number of teams ever created.

Robust Memory Management: Eliminated leaks with thorough object deallocation paths and explicit checks, verified via Valgrind.

Algorithmic Proofs: Provided formal justification of complexity, demonstrating near‐constant union–find merges and stable hash performance.

By combining union–find merges with targeted hash‐table indexing, the project successfully delivered a system capable of handling dynamic merges, record updates, and queries in constant‐time operations—fulfilling stringent performance and space criteria.
