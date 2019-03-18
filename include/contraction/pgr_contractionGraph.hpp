/*PGR-GNU*****************************************************************
File: pgr_contractionGraph.hpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2016 Rohith Reddy
Mail:

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

#ifndef INCLUDE_CONTRACTION_PGR_CONTRACTIONGRAPH_HPP_
#define INCLUDE_CONTRACTION_PGR_CONTRACTIONGRAPH_HPP_
#pragma once

#include <boost/graph/iteration_macros.hpp>

#include <limits>
#include <algorithm>
#include <vector>
#include <iostream>

#include "cpp_common/pgr_base_graph.hpp"
#include "cpp_common/ch_vertex.h"
#include "cpp_common/ch_edge.h"


namespace pgrouting {
namespace graph {

template <class G, typename T_V, typename T_E>
class Pgr_contractionGraph : public Pgr_base_graph<G, T_V, T_E> {
 public:
     typedef typename boost::graph_traits < G >::vertex_descriptor V;
     typedef typename boost::graph_traits < G >::edge_descriptor E;
     typedef typename boost::graph_traits < G >::vertex_iterator V_i;
     typedef typename boost::graph_traits < G >::edge_iterator E_i;
     typedef typename boost::graph_traits < G >::out_edge_iterator EO_i;
     typedef typename boost::graph_traits < G >::in_edge_iterator EI_i;
     typedef typename boost::graph_traits < G >::degree_size_type
         degree_size_type;

     Identifiers<V> removed_vertices;
     Identifiers<E> shortcuts;

#if 0
     /*! @brief Binary function that accepts two elements , and returns a value convertible to bool.
       Used as a compare function to sort the edges in increasing order of edge id
       */
     static bool compareById(const T_E &edge1, const T_E &edge2) {
         return edge1.id > edge2.id;
     }
#endif
     /*!
       Prepares the _graph_ to be of type *gtype*
       */
     explicit Pgr_contractionGraph< G , T_V, T_E >(graphType gtype)
         : Pgr_base_graph< G , T_V, T_E >(gtype) {
         }

     /*! @brief get the vertex descriptors of adjacent vertices of *v*
       @param [in] v vertex_descriptor
       @return Identifiers<V>: The set of vertex descriptors adjacent to the given vertex *v*
       */
     Identifiers<V> find_adjacent_vertices(V v) const {
         EO_i out, out_end;
         EI_i in, in_end;
         Identifiers<V> adjacent_vertices;

         for (boost::tie(out, out_end) = out_edges(v, this->graph);
                 out != out_end; ++out) {
             adjacent_vertices += this->adjacent(v, *out);
         }
         for (boost::tie(in, in_end) = in_edges(v, this->graph);
                 in != in_end; ++in) {
             adjacent_vertices += this->adjacent(v, *in);
         }
         return adjacent_vertices;
     }

#if 0
     std::vector<int64_t> get_ids(
             Identifiers<int64_t> boost_ids) const {
         std::vector<int64_t> ids(boost_ids.size());
         size_t count = 0;
         for (auto id : boost_ids) {
             ids[count++] = this->graph[id].id;
         }
         return ids;
     }
#endif

#if 0
     /*! @brief vertices with at least one contracted vertex
      *
       @result The vids Identifiers with at least one contracted vertex
       */
     Identifiers<int64_t> get_changed_vertices() {
        Identifiers<int64_t> vids;
         for (auto vi = vertices(this->graph).first;
                 vi != vertices(this->graph).second;
                 ++vi) {
             if (!removed_vertices.has(*vi)
                     && this->graph[*vi].has_contracted_vertices()) {
                 vids += this->graph[*vi].id;
             }
         }
         return vids;
     }
#endif

     /*! @brief get the edge with minimum cost between two vertices
       @param [in] u vertex_descriptor of source vertex
       @param [in] v vertex_descriptor of target vertex
       @return E: The edge descriptor of the edge with minimum cost
       */
     std::tuple<E, Identifiers<int64_t>, bool> get_min_cost_edge(V u, V v) {
         E min_edge;
         Identifiers<int64_t> contracted_vertices;
         double min_cost = (std::numeric_limits<double>::max)();
         bool found = false;

         if (this->is_directed()) {
             BGL_FORALL_OUTEDGES_T(u, e, this->graph, G) {
                 if (this->target(e) == v) {
                     contracted_vertices = this->graph[e].contracted_vertices();
                     if (this->graph[e].cost < min_cost) {
                         min_cost = this->graph[e].cost;
                         min_edge = e;
                         found = true;
                     }
                 }
             }
             return std::make_tuple(min_edge, contracted_vertices, found);
         }

         pgassert(this->is_undirected());
         BGL_FORALL_OUTEDGES_T(u, e, this->graph, G) {
             if (this->adjacent(u, e) == v) {
                 contracted_vertices = this->graph[e].contracted_vertices();
                 if (this->graph[e].cost < min_cost) {
                     min_cost = this->graph[e].cost;
                     min_edge = e;
                     found = true;
                 }
             }
         }
         return std::make_tuple(min_edge, contracted_vertices, found);
     }

#if 0
     /*! @brief The number of edges from @b neighbor to @b vertex

       @param [in] vertex is the target of the edges
       @param [in] neighbor is the source of the edges
       @return degree_size_type: The in-degree of *vertex* from *neighbor*
       */
     degree_size_type in_degree_from_vertex(V vertex, V neighbor) {
         return out_degree_to_vertex(neighbor, vertex);
     }
#endif
#if 0
     /*! @brief The number of edges from @b vertex to @b neighbor

       @param [in] vertex vertex_descriptor of the given vertex
       @param [in] neighbor vertex_descriptor of neighbor
       @return degree_size_type: The out-degree of *vertex* to *neighbor*
       */
     degree_size_type out_degree_to_vertex(V vertex, V neighbor) {
         degree_size_type degree = 0;
         EO_i out_i, out_end;
         for (boost::tie(out_i, out_end) =
                 boost::out_edges(vertex, this->graph);
                 out_i != out_end; ++out_i) {
             if (this->is_directed()
                     && (this->is_source(vertex, *out_i)
                         && this->is_target(neighbor, *out_i))) {
                 degree++;
             } else if (this->is_undirected() &&
                     this->adjacent(vertex, *out_i) == neighbor) {
                 degree++;
             }
         }
         return degree;
     }
#endif

     /*! @brief print the graph with contracted vertices of
       all vertices and edges
       */
     friend
     std::ostream& operator <<(
             std::ostream &os,
             const Pgr_contractionGraph &g) {
         EO_i out, out_end;
         for (auto vi = vertices(g.graph).first;
                 vi != vertices(g.graph).second;
                 ++vi) {
             if ((*vi) >= g.num_vertices()) break;
             os << g.graph[*vi].id << "(" << (*vi) << ")"
                 << g.graph[*vi].contracted_vertices() << std::endl;
             os << " out_edges_of(" << g.graph[*vi].id << "):";
             for (boost::tie(out, out_end) = out_edges(*vi, g.graph);
                     out != out_end; ++out) {
                 os << ' ' << g.graph[*out].id
                     << "=(" << g.graph[g.source(*out)].id
                     << ", " << g.graph[g.target(*out)].id << ") = "
                     <<  g.graph[*out].cost <<"\t";
             }
             os << std::endl;
         }
         return os;
     }


     /*! @brief get the contracted vertex ids of a given vertex in array format
       @param [in] vid vertex_id

       @returns ids of contracted_vertices
       */
     std::vector<int64_t> get_contracted_vertices(int64_t vid) {
         if (!this->has_vertex(vid)) return std::vector<int64_t>();
         auto  v = this->get_V(vid);
         std::vector<int64_t> ids(this->graph[v].contracted_vertices().size());

         size_t count = 0;
         for (auto idx :  this->graph[v].contracted_vertices()) {
             ids[count++] =  idx;
         }
         return ids;
     }




#if 0
     /*! @brief add the contracted vertices of an edge *e* to the vertex *v*
       @param [in] v vertex_descriptor
       @param [in] e Edge of type *T_E*
       */
     void add_contracted_edge_vertices(V v, T_E &e) {
         for (auto vid : e.contracted_vertices()) {
             this->graph[v].add_vertex_id(vid);
         }
         e.clear_contracted_vertices();
     }
#endif

     /*! @brief add edges(shortuct) to the graph during contraction

       u -> v -> w

       u -> w

       edge (u, w) is a new edge e
       e.contracted_vertices = v + v.contracted vertices

       removed from graph edges: u -> v  and v -> w


       @param [in] edge of type *T_E* is to be added
       */

     void add_shortcut(const T_E &edge) {
         bool inserted;
         E e;
         if (edge.cost < 0)
             return;

         pgassert(this->vertices_map.find(edge.source)
                 != this->vertices_map.end());
         pgassert(this->vertices_map.find(edge.target)
                 != this->vertices_map.end());

         auto vm_s = this->get_V(edge.source);
         auto vm_t = this->get_V(edge.target);

         boost::tie(e, inserted) =
             boost::add_edge(vm_s, vm_t, this->graph);

         this->graph[e].cp_members(edge);

         shortcuts += e;
     }

     bool is_contracted(V v) {
        return this->graph[v].has_contracted_vertices();
     }

#if 0
     bool is_contracted(E e) {
        return this->graph[e].id < -1;
     }
#endif
     void get_remaining_vertices(Identifiers<int64_t>& remaining_vertices) {
        for (auto vi = vertices(this->graph).first;
                vi != vertices(this->graph).second;
                ++vi) {
            if (is_contracted(*vi)) {
                remaining_vertices += this->graph[*vi].id;
            }
        }
     }

     Identifiers<E> get_shortcuts() {
         return shortcuts;
     }
#if 0
     void get_shortcuts(std::vector< CH_edge >& shortcut_edges) {
         EO_i out, out_end;
         CH_edge e;
         for (auto vi = vertices(this->graph).first;
                 vi != vertices(this->graph).second;
                 ++vi) {
             if ((*vi) >= this->num_vertices()) break;

            for (boost::tie(out, out_end) = out_edges(*vi, this->graph);
                    out != out_end; ++out) {
                if (is_contracted(*out)) {
                    e.id = this->graph[*out].id;
                    e.source = this->graph[this->source(*out)].id;
                    e.target = this->graph[this->target(*out)].id;
                    e.cost = this->graph[*out].cost;
                    e.add_contracted_edge_vertices(this->graph[*out]);
                    shortcut_edges.push_back(e);
                }
            }
        }
     }
#endif
};

}  // namespace graph
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_PGR_CONTRACTIONGRAPH_HPP_
