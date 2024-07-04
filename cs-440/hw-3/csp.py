#
#  Constraint Statisfaction for map coloring!
#

class CSP():

    def __init__(self, graph, domains):
        """ Initialize a graph-coloring problem:

        Arguments:
          graph: a map from int -> set of integers
                that represents the graph in an
                adjacency set format.
                That is, each vertex in the graph
                has a integer index from 0...(nvertices-1)
                and the set stored at graph[i] represents
                all destination vertices from outgoing edges
                from vertex i.  In this representation, an
                undirected edge is represented as two directional
                edges: one from i to j and one from j to i.
                Further, it is not possible with this representation
                to have two distinct edges from i to j.

          domains: a map from int -> set where each element in
                the set stored at domain[i] indicates possible
                values that the vertex i may consider. e.g.,
                if domains[0] = { 'R', 'G', 'B' }, then vertex
                0 in the graph could be colored 'R', 'G', or 'B'

        """
        self.graph = graph
        self.domains = domains
        self.nvertices = len(self.graph)

    def solve(self, solutions=1, count=False, allstats=False, quiet=True):
        """_ Part 1: Implement this method _

        Arguments:
          solutions - the desired number of solutions to return.
                      it is possible that the search space will be
                      exhausted prior to finding the desired number
                      of solutions in which case all found solutions
                      will be returned.

          count - True if you should fully explore the state space
                  and return a count of legal solutions.  False if
                  you should terminate early, when the desired number
                  of solutions is found.

          allstats - True if you should return information about the
                  number of states that are pruned during search.
                  Supporting this argument is *optional*

          quiet - if True, don't print any output; there is no
                  need to do anything special if quiet == False,
                  but you should obey quiet == True by ensuring
                  no output is generated.
        

        Returns:
          a dictionary with keys dependent on the arguments:
          solutions, count and allstats:

          if solutions is > 0:
          'Solutions': should be a list of sequences; each sequence a
                       legal domain value for each vertex in the graph.
                       The length of the 'Solutions' list should be
                       == the solutions argument unless fewer solutions
                       actually exist in the search space. In that case,
                       all solutions should be included in the list.

          if count == True:
          'SolutionCount': the number of fully specified states that are valid
                           solutions

          if allstats == True:
          'Unexplored' : the number of fully specified states that have not
                         been deemed a solution or rejected
          'Rejected': the number of fully specified states that have been
                       eliminated as solutions
          'Enqueued': the number of *partial* states that have been put
                      on the queue

          Note that when count == True,
           The each state in the state space must either be a solution or rejected.
          when count == False,
           it is possible that a solution is returned while some fully specified
           states remain unexplored.
        """
        
        '''while(True):
            solution_list = list()
            for i in range(solutions):
                tmp_lst = list()
                possible_vals = list()
                index = 0
                # make an empty list for solution and default list for accepted
                for i in range(len(self.graph)):
                    tmp_list.push(None)
                    possible_vals.push(self.domain)
                # start doesn't matter, if one start is invalid all will be
                # prunes majority of branches by default
                # could skip second step as well if following path and not index order
                tmp_lst[0] = domain[0]
                while(index < len(self.graph)):
                    for i in self.graph[index]:
                        possible_vals[i].remove(tmp_lst[i])
                        if len(possible_vals[i] == 0):
                            # no legal moves, go back a step and try again
                        else:
                            tmp_lst[i] = possible_vals[i][0]'''
        allowed = self.domains
        sol = list()
        for i in range(len(self.graph)):
            sol.append(None)
            allowed[i] = list(allowed[i])
        #print(self.domains[0])
        sol[0] = self.domains[0][0]
        allowed[0] = sol[0]
        print(solutions)
        result = self.__search(sol, allowed, 0, list(), 0, 0)
        #print("-------------------------------------")
        print(result)
        #print("-------------------------------------")
        # build final dict to return
        final = dict()
        if (None not in result[0]):
            final['Solutions'] = result[0][:solutions]
            for i in range(len(self.domains[0])):
                tmp_domain = self.domains[0][i+1:] + self.domains[0][:i+1]
                for ent in final['Solutions']:
                    tmp_sol = ent
                    for j in range(len(ent)):
                        for k in range(len(self.domains[0])):
                            if ent[j] == self.domains[0][k]:
                                ent[j] = tmp_domain[k]
                                break
                    print(tmp_sol)
                    if tmp_sol not in final['Solutions']:
                        final['Solutions'].append(tmp_sol)

        else:
            final['Solutions'] = None
        if count:
            final["SolutionCount"] = len(result[0])
        if allstats:
            max_pos = len(self.domains)**len(self.graph)
            # divide out top most node to account for shortcut
            final["Unexplored"] = max_pos/len(self.domains) - result[1]
            final["Rejected"] = 0 # TODO: count rejected
            final["Enqueued"] = result[2]
        print(final)
        return final

    # recursive search function
    #
    # @param {List} current state of graph
    # @param {int} index to check
    # @param {[List]} list of remaining allowed states for each node on graph
    # @param {int} count of nodes visited
    #
    # @return {tuple} result of pick, state of graph, allowed list, count of nodes visited
    '''def __search(self, state, index, allowed, count, solutions_found, list_sols):
        not_quiet = True
        tmp_domain = allowed[index]

        if not_quiet:
            print("index: "+str(index))
            print("count: "+str(count))
            print("state: "+str(state))
            print("allowed: "+str(allowed))
            print("solutions found: "+str(solutions_found))
            print("----------")
            print(tmp_domain)

        for ent in tmp_domain:
            # tmp allowed, so not re-inserting if bad pick is made
            allowed_t = allowed
            valid_pick = True

            if not_quiet:
                print(self.graph[index])

            # check every node adjacent to index node
            for i in self.graph[index]:
                # if
                if (not state[i]) and (ent in allowed_t[i]):
                    if not_quiet:
                        print(i)
                        print("removing: "+str(ent))
                        print(allowed_t)
                        print(allowed_t[i])
                    allowed_t[i].remove(ent)
                    if len(allowed_t[i]) == 0:
                        valid_pick = False
                        break
                #if len(allowed_t[i]) == 0:
                #    valid_pick = False
                #    break

            if not valid_pick:
                if not_quiet:
                    print(allowed)
                    print(allowed[index])
                # remove bad pick from allowed
                allowed[index].remove(ent)
                if not_quiet:
                    print("not valid pick")
                continue

            if not_quiet:
                print("set sol["+str(index)+"] to "+str(ent))

            state[index] = ent
            # no placeholder values in state, must be good
            if None not in state:
                if state not in list_sols:
                    list_sols.append(state)
                    solutions_found +=1
                #if solutions_found == num_solutions:
                #    return ("valid", state, allowed, count, solutions_found, list_sols)
                #else:
                allowed_t[index].remove(ent)
                return ("bad", state, allowed_t, count, solutions_found, list_sols)
            if not not_quiet:
                print(allowed_t)
            result = self.__search(state, index+1, allowed_t, count+1, solutions_found, list_sols)
            if not_quiet:
                print("result"+str(result))
            # keep count up to date
            count = result[3]
            # end loop if returned good result

            if result[0] == "valid":
                return result

            # remove bad pick from allowed if sub tree rejects all states
            if allowed[index] == ent:
                allowed[index] = []
            else:
                allowed[index].remove(ent)

        # no valid moves, go back one step
        return ("bad", state, allowed, count, solutions_found, list_sols)'''

    def __search(self, current_state, allowed, index, solutions, visit_count, reject_count):
        not_quiet = True

        # if passed end of graph, end early
        if index >= len(self.graph):
            if current_state not in solutions:
                solutions.append(current_state)
            return (solutions, visit_count, reject_count)
        
        # check every possible color for current node
        for color in allowed[index]:
            # if bad path, revert to last known good state of allowed
            allowed_tmp = allowed

            valid_path = True

            # count node
            visit_count += 1

            # check every adjacent node
            for neighbor in self.graph[index]:
                if valid_path:
                    # remove color from neighbor, if no possible colors for neighbor revert
                    if color in allowed_tmp[neighbor]:
                        allowed_tmp[neighbor].remove(color)
                    if len(allowed_tmp) == 0:
                        valid_path = False
                        break

            # if bad pick increment rejected and continue to next color
            if not valid_path:
                reject_count += 1
                continue
            # otherwise pick was good, go deeper
            else:
                tmp_state = current_state
                tmp_state[index] = color
                update = self.__search(tmp_state, allowed_tmp, index+1, solutions, visit_count, reject_count)
                # updated based on recursive return
                solutions = update[0]
                visit_count = update[1]
                reject_count = update[2]
                # continue to check next color
        # at end of loop, return
        return (solutions, visit_count, reject_count)

            




def load_graph_file(fname):
    with open(fname, 'rt') as fin:
        graphstr = fin.read()
        return graphstr


def parse_graph_str(graphstr):
    lines = graphstr.splitlines()
    nv = int(lines[0].strip())
    graph = {i: {} for i in range(nv)}
    for line in lines[1:]:
        (v, e) = line.split(';')
        v = int(v.strip())
        e = [int(_) for _ in e.split()]
        graph[v] = e
    return graph


def parse_domains(domainstr):
    domains = {}
    for line in domainstr.splitlines():
        v, d = line.split(':')
        domains[int(v.strip())] = set(d.strip())
    return domains


if __name__ == "__main__":
    #
    # count the number of solutions to a simple problem:
    # when the graph in the file 'graph.g' is :
    # 4
    # 0; 1 2
    # 1; 0 2 3
    # 2; 0 1 3
    # 3; 1 2
    #
    # and each node is given 3 colors:
    # output (count=True) is (note that your Solution may be different!):
    #
    # Done in : 0.0005519390106201172
    # {'Solutions': [['B', 'G', 'R', 'B']], 'SolutionCount': 6}

    import time

    graph = parse_graph_str(load_graph_file('graph.g'))

    def makedomains(ncolors, graph):
        """Return map from vertex id -> set of color codes"""
        colors = 'RGBCMYKOPASDF'
        if ncolors > len(colors):
            print("That's too many colors ;)")
            return None
        return {k: set(colors[:ncolors]) for k in graph}

    domains = makedomains(3, graph)
    mycsp = CSP(graph, domains)

    start = time.time()
    r = mycsp.solve(count=True, quiet=True)
    stop = time.time()

    print("Done in :", (stop-start))

    print(r)

