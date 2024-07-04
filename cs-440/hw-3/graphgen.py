"""
Functions to make random graphs.
Functions to read/write graphs, functions for creating domains.
"""
import random

def generategraph_m1(nverts, minfanout, maxfanout, avgfanout, tolerance):
    """Generate a graph with nverts vertices."""

    _vertices = {i for i in range(nverts)}
    graph = {i: [] for i in _vertices}
    targetfanout = {i: random.randint(minfanout, maxfanout) for i in range(nverts) }

    targetvalue = sum(v for v in targetfanout.values()) / len(targetfanout)
    print("Current fanout target is: ", targetvalue, " ", avgfanout)
    vertices_needing_more_edges = list(_vertices)
    print(targetfanout)
    while vertices_needing_more_edges:
        i = vertices_needing_more_edges.pop()
        # pick some edge destinations from the list of who needs more
        if len(vertices_needing_more_edges) < targetfanout[i]:
            print("No more destinations remain!")
            break

        dests = random.sample(vertices_needing_more_edges, targetfanout[i])
        for d in dests:
            graph[i].append(d)
            graph[d].append(i)
            targetfanout[d] = targetfanout[d] - 1
            if targetfanout[d] == 0:
                vertices_needing_more_edges.remove(d)

    actualvalue = sum(len(v) for v in graph.values()) / len(targetfanout)
    print("Actual fanout value is: ", actualvalue)
    if actualvalue < avgfanout - tolerance:
        # pump up.
        vertices_that_could_increase = [i for i in _vertices if len(graph[i]) < maxfanout]
        while actualvalue < avgfanout - tolerance and len(vertices_that_could_increase) >= 2:
            # two vertices, and connect them
            vi, vj = random.sample(vertices_that_could_increase, 2)
            graph[vi].append(vj)
            graph[vj].append(vi)
            if len(graph[vi]) == maxfanout:
                vertices_that_could_increase.remove(vi)
            if len(graph[vj]) == maxfanout:
                vertices_that_could_increase.remove(vj)
            # update average
            actualvalue = ((actualvalue * nverts) + 2) / nverts

    if actualvalue > avgfanout + tolerance:
        # drop down.
        vertices_that_could_decrease = [i for i in _vertices if len(graph[i]) > minfanout]
        while actualvalue > avgfanout + tolerance and len(vertices_that_could_decrease) >= 2:
            # pick one vertex, remove an edge
            vi = random.choice(vertices_that_could_decrease)
            # which edge?
            possible_removals = [vj for vj in graph[vi] if len(graph[vj]) > minfanout]
            if not possible_removals:
                # has enough edges, but the connected vertices are all at their mins
                vertices_that_could_decrease.remove(vi)
                continue
            vj = random.choice(possible_removals)

            graph[vi].remove(vj)
            graph[vj].remove(vi)
            if len(graph[vi]) <= minfanout:
                vertices_that_could_decrease.remove(vi)
            if len(graph[vj]) <= minfanout:
                vertices_that_could_decrease.remove(vj)
            # update average
            actualvalue = ((actualvalue * nverts) - 2) / nverts

    print("Final fanout value is: ", actualvalue)
    return graph


def write_graph(graph, fname):
    """Write a graph to a file in a format that is human readable, and loadable."""

    with open(fname, 'wt') as fout:
        fout.write("%d\n" % len(graph))
        for k in sorted(graph.keys()):
            # only write vertices with outgoing edges
            if graph[k]:
                fout.write("%d;" % k)
                for v in graph[k]:
                    fout.write(" %d" % (v))
                fout.write("\n")


if __name__ == "__main__":
    g = generategraph_m1(15, 3, 6, 4, .2)
    write_graph(g, "graph.g")
