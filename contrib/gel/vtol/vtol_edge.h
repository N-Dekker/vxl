// This is gel/vtol/vtol_edge.h
#ifndef vtol_edge_h
#define vtol_edge_h
//:
// \file
// \brief Represents the basic 1D topological entity
//
//  The vtol_edge class is used to represent a topological edge.  An vtol_edge
//  maintains a data pointer to the specific mathematical curve geometry
//  which describes the point set that makes up the edge.  For convenience
//  in working with linear edges, pointers to the two endpoint vertices
//  are maintained. The direction of an edge is the vector from v1_ to v2_.
//  A OneChain is the Superior of the edge in the topological
//  hierarchy, and a ZeroChain is the Inferior of the edge in the
//  topological hierarchy.  In rare cases, an edge will be used to represent
//  a Ray.  In this case, only v1_ will be valid and v2_ will be NULL.
//
// \verbatim
// Modifications:
//     JLM December 1995, Added timeStamp (Touch) to
//         operations which affect bounds.
//     JLM December 1995 Added method for ComputeBoundingBox
//         (Need to decide proper policy for curved edges
//         and possibly inconsistent linear edge geometry)
//
//     Samer Abdallah - 21/06/1996
//      Robotics Research Group, Oxford
//      Changed the constructor vtol_edge(vtol_edge &) to vtol_edge(const vtol_edge &)
//
//     JLM September 1996 - Added default curve argument to two vertex
//     constructors.  This addition is necessary because it is not
//     always the case that one wants to construct an ImplicitLine from
//     two vertices.  The curve might be a DigitalCurve, for example.
//     On the other hand in grouping or similar applications, the
//     curve endpoints can be different from the topological connections.
//     So, it is necessary to pass in the vertices as well as the curve.
//
//   02-26-97 Added implementation for virtual Transform() - Peter Vanroose
//   PTU ported to vxl may 2000.
// \endverbatim

#include <vtol/vtol_topology_object.h>
#include <vcl_vector.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_vertex.h>
class vtol_edge_2d;

//: topological edge

class vtol_edge
  : public vtol_topology_object
{
 protected:

  // Keeping vertex pointers inside of edge
  // for convenience...for now.

  vtol_vertex_sptr v1_;
  vtol_vertex_sptr v2_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default constructor. Empty edge. Not a valid edge.
  //---------------------------------------------------------------------------
  explicit vtol_edge(void):v1_(0),v2_(0){ link_inferior(new vtol_zero_chain); }

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vtol_edge();

  //---------------------------------------------------------------------------
  //: Return the topology type
  //---------------------------------------------------------------------------
  virtual vtol_topology_object_type topology_type(void) const { return EDGE; }

  //---------------------------------------------------------------------------
  //: Return the first endpoint
  //---------------------------------------------------------------------------
  vtol_vertex_sptr v1(void) const { return v1_; }

  //---------------------------------------------------------------------------
  //: Return the second endpoint
  //---------------------------------------------------------------------------
  vtol_vertex_sptr v2(void) const { return v2_; }

  //---------------------------------------------------------------------------
  //: Return the first non-empty zero-chain of `this'
  //---------------------------------------------------------------------------
  virtual vtol_zero_chain_sptr zero_chain(void) const;

  //---------------------------------------------------------------------------
  //: Set the first endpoint.
  //---------------------------------------------------------------------------
  virtual void set_v1(vtol_vertex_sptr new_v1);

  //---------------------------------------------------------------------------
  //: Set the last endpoint
  //---------------------------------------------------------------------------
  virtual void set_v2(vtol_vertex_sptr new_v2);

  //---------------------------------------------------------------------------
  //: Determine the endpoints of an edge from its inferiors
  //---------------------------------------------------------------------------
  virtual void set_vertices_from_zero_chains(void);

  //---------------------------------------------------------------------------
  //: replace the current end point
  //---------------------------------------------------------------------------
  virtual void replace_end_point(vtol_vertex &current_end_point,
                                 vtol_vertex &new_end_point);

  virtual bool operator==(const vtol_edge &other) const;
  inline bool operator!=(const vtol_edge &other)const{return !operator==(other);}
  bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  virtual void add_edge_loop(vtol_one_chain &);
  virtual void remove_edge_loop(vtol_one_chain &);

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge *cast_to_edge(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge *cast_to_edge(void) { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_edge_2d *cast_to_edge_2d(void) const {return 0;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an edge, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_edge_2d *cast_to_edge_2d(void) {return 0;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  void link_inferior(vtol_zero_chain_sptr inf);
  void unlink_inferior(vtol_zero_chain_sptr inf);

  //---------------------------------------------------------------------------
  //: Is `inferior' type valid for `this' ?
  //---------------------------------------------------------------------------
  virtual bool valid_inferior_type(vtol_topology_object const* inferior) const
  { return inferior->cast_to_zero_chain() != 0; }
  bool valid_inferior_type(vtol_zero_chain_sptr ) const { return true; }
  bool valid_superior_type(vtol_one_chain_sptr ) const { return true; }

  //:
  // Inferior/Superior Accessor Methods
  //
  // WARNING should not be used by clients
  virtual vcl_vector<vtol_vertex*> *compute_vertices(void);
  virtual vcl_vector<vtol_edge*> *compute_edges(void);
  virtual vcl_vector<vtol_zero_chain*> *compute_zero_chains(void);
  virtual vcl_vector<vtol_one_chain*> *compute_one_chains(void);
  virtual vcl_vector<vtol_face*> *compute_faces(void);
  virtual vcl_vector<vtol_two_chain*> *compute_two_chains(void);
  virtual vcl_vector<vtol_block*> *compute_blocks(void);

  //: get a list of endpoints
  virtual vertex_list *endpoints(void);

  // Utility Functions

  virtual bool share_vertex_with(vtol_edge &other);

  virtual bool add_vertex(vtol_vertex &);
  virtual bool remove_vertex(vtol_vertex &);

  virtual bool is_endpoint(const vtol_vertex &) const;
  virtual bool is_endpoint1(const vtol_vertex &) const;
  virtual bool is_endpoint2(const vtol_vertex &) const;

  virtual vtol_vertex_sptr other_endpoint(const vtol_vertex &) const;

  virtual void print(vcl_ostream &strm=vcl_cout) const;
  virtual void describe(vcl_ostream &strm=vcl_cout,
                        int blanking=0) const;

  //: compare the geometry
  virtual bool compare_geometry(const vtol_edge &other) const =0;
};

#endif // vtol_edge_h
