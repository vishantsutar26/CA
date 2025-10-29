# Bellman-Ford Algorithm Visualization in SageMath (with more cycles)

# Create a weighted directed graph
G = DiGraph()

# Add edges (source, destination, weight)
G.add_edges([
    ('A', 'B', 4),
    ('A', 'C', 2),
    ('B', 'C', 3),
    ('B', 'D', 2),
    ('B', 'E', 3),
    ('C', 'B', 1),     # Cycle: B → C → B
    ('C', 'D', 4),
    ('D', 'E', -1),
    
    # Extra edges to increase cycles:
    ('E', 'A', 2),     # Cycle: A → B → E → A
    ('D', 'A', 1),     # Cycle: A → B → D → A
    ('E', 'C', 1),     # Cycle: C → D → E → C
    ('D', 'C', 2),     # Cycle: C → D → C
])

# Display basic graph
G.show(edge_labels=True, layout='spring', figsize=6, vertex_size=600, vertex_labels=True)

# Source vertex
source = 'A'
print(f"🔹 Running Bellman-Ford from source: {source}\n")

# Initialize distances and predecessors
distance = {v: float('inf') for v in G.vertices()}
predecessor = {v: None for v in G.vertices()}
distance[source] = 0

# Relax edges |V|-1 times
for i in range(len(G.vertices()) - 1):
    for (u, v, w) in G.edges():
        if distance[u] + w < distance[v]:
            distance[v] = distance[u] + w
            predecessor[v] = u

# Detect negative-weight cycles
negative_cycle_edges = []
for (u, v, w) in G.edges():
    if distance[u] + w < distance[v]:
        print(f"⚠️ Negative weight cycle detected via edge ({u} → {v}) with weight {w}")
        negative_cycle_edges.append((u, v))

# Print shortest distances
print("\n📏 Shortest Distances from", source)
for v, d in distance.items():
    print(f"  {source} → {v}: {d}")

# Print predecessor table
print("\n🧭 Predecessors")
for v, p in predecessor.items():
    print(f"  {v} ← {p}")

# Function to reconstruct path from source to any vertex
def reconstruct_path(v):
    if distance[v] == float('inf'):
        return None  # unreachable
    path = []
    while v is not None:
        path.insert(0, v)
        v = predecessor[v]
    return path

# Print all shortest paths
print("\n🛤️ Final Shortest Paths from", source)
for v in G.vertices():
    if v == source:
        continue
    path = reconstruct_path(v)
    if path:
        print(f"  {source} → {v}: {' → '.join(path)} (Cost = {distance[path[-1]]})")
    else:
        print(f"  {source} → {v}: Unreachable")

# Collect shortest path tree edges
shortest_path_edges = [(predecessor[v], v) for v in G.vertices() if predecessor[v] is not None]

# Get layout for consistent positions
pos = G.layout('spring')

# Base graph
base_plot = G.plot(
    pos=pos,
    edge_labels=True,
    vertex_size=600,
    vertex_labels=True,
    edge_color='gray',
    figsize=6
)

# Highlight shortest paths in red
highlight_shortest = sum([line([pos[e[0]], pos[e[1]]], color='red', thickness=3) for e in shortest_path_edges])

# Highlight negative cycle edges in orange dashed
highlight_negative = sum([line([pos[e[0]], pos[e[1]]], color='orange', thickness=4, linestyle='--') for e in negative_cycle_edges])

# Combine plots
final_plot = base_plot + highlight_shortest + highlight_negative

# Show the final visualization
final_plot.show(figsize=6)
