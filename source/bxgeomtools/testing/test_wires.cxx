// test_wires.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <list>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

// This project:
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/line_3d.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/union_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/subtraction_3d.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'geomtools::parse_wires'." << std::endl;

    bool draw = false;
    bool delta_z = false;
    bool horiz = false;
    bool cyl2 = false;
    bool inter = false;
    bool subtr = false;
    bool shift2 = false;
    bool plot_polylines = false;
    bool plot_shapes = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-Z") delta_z = true;
      if (arg == "-H") horiz = true;
      if (arg == "-C2") cyl2 = true;
      if (arg == "-pl") plot_polylines = true;
      if (arg == "-sh") plot_shapes = true;
      if (arg == "-S2") shift2 = true;
      if (arg == "-I") {
        inter = true;
        subtr = false;
      }
      if (arg == "-S") {
        subtr = true;
        inter = false;
      }
     iarg++;
    }

    geomtools::box b1;
    b1.set(2.0, 2.0, 2.0);
    b1.lock();

    geomtools::box b2;
    b2.set(2.0, 2.0, 2.0);
    b2.lock();

    geomtools::cylinder c2;
    c2.set(1.0, 2.0);
    c2.lock();

    geomtools::union_3d u1;
    geomtools::intersection_3d i1;
    geomtools::subtraction_3d s1;
    geomtools::placement up1;
    up1.set_identity();
    // up1.set(-0.5, 0.0, 0.0,
    //         0.0 * CLHEP::degree,
    //         0.0 * CLHEP::degree,
    //         0.0 * CLHEP::degree);
    geomtools::placement up2(2.0, -0.75, 0.0,
                             45 * CLHEP::degree,
                             30.0 * CLHEP::degree,
                             0.0 * CLHEP::degree);
    double zb2 = 0.0;
    if (shift2) zb2 = 0.5;
    up2.set(2.0 - 0.1, 0.0, zb2,
            36.0 * CLHEP::degree,
            0.0 * CLHEP::degree,
            0.0 * CLHEP::degree);
    geomtools::i_composite_shape_3d * comp1 = &u1;
    if (inter) {
      comp1 = &i1;
    }
    if (subtr) {
      comp1 = &s1;
    }
    if (!cyl2) {
      comp1->set_shapes(b1, b2, up2);
    } else {
      comp1->set_shapes(b1, c2, up2);
    }
    comp1->lock();

    geomtools::line_3d segment1;
    double z1 = 0.0;
    double z2 = 0.0;
    double eps = 0.002;
    eps = 0.0;
    if (horiz) {
      z1 += (1.0 - eps);
      z2 += (1.0 - eps);
    } else if (delta_z) {
      z1 += (1.0);
      z2 -= (1.0);
    }
    segment1.set_first(geomtools::vector_3d(-2.0, 1.0, z1));
    segment1.set_last(geomtools::vector_3d(+4.0, -0.5, z2));

    geomtools::polyline_3d p1;
    p1.set_closed(false);
    p1.add(geomtools::vector_3d(0.0, 0.0, 1.5));
    p1.add(geomtools::vector_3d(0.5, 0.0, 1.5));
    p1.add(geomtools::vector_3d(1.0, 0.5, 1.5));
    p1.add(geomtools::vector_3d(1.0, 1.0, 1.5));
    p1.add(geomtools::vector_3d(0.5, 1.5, 1.5));
    p1.add(geomtools::vector_3d(0.0, 1.5, 1.5));
    p1.add(geomtools::vector_3d(0.0, 0.0, 1.5));

    geomtools::polyline_3d p2;
    p2.set_closed(false);
    p2.add(geomtools::vector_3d(0.0, 0.0, 1.5));
    p2.add(geomtools::vector_3d(0.0, 0.0, 2.5));
    p2.add(geomtools::vector_3d(0.5, 0.0, 2.5));
    p2.add(geomtools::vector_3d(0.5, 0.0, 1.5));
    p2.add(geomtools::vector_3d(0.0, 0.0, 1.5));

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_wires_");

    geomtools::vector_3d origin(0.0, 0.0, 0.0);
    geomtools::rotation_3d identity(0.0, 0.0, 0.0);

    std::ostringstream buffer;
    buffer << "#p1\n";
    geomtools::gnuplot_draw::draw_polyline(buffer,
                                           origin,
                                           identity,
                                           p1, p1.is_closed(), true);
    buffer << "#p2\n";
    geomtools::gnuplot_draw::draw_polyline(buffer,
                                           origin,
                                           identity,
                                           p2, p2.is_closed(), true);

    // Original polylines: p1 and p2 (index 0):
    if (plot_polylines) {
      tmp_file.out() << "# Original polylines:" << std::endl;
      geomtools::vector_3d pos(0.0, 0.0, 0.5);
      geomtools::gnuplot_draw::draw_polyline(tmp_file.out(),
        pos,
        identity,
        p1, p1.is_closed(), true);
      geomtools::gnuplot_draw::draw_polyline(tmp_file.out(),
        pos,
        identity,
        p2, p2.is_closed(), true);
      tmp_file.out() << std::endl;
    }

    // Boxes/cylinder: b1 and b2/c2 (index 1):
    tmp_file.out() << "# Boxes/cylinder:" << std::endl;
    if (plot_shapes) {
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
        up1.get_translation(),
        up1.get_rotation(),
        b1);
      if (!cyl2) {
        geomtools::gnuplot_draw::draw_box(tmp_file.out(),
          up2.get_translation(),
          up2.get_rotation(),
          b2);
      } else {
        geomtools::gnuplot_draw::draw_cylinder(tmp_file.out(),
          up2.get_translation(),
          up2.get_rotation(),
          c2);
      }
      tmp_file.out() << std::endl;
    }

    // Segment: segment1 (index 2):
    tmp_file.out() << "# Segment:" << std::endl;
    geomtools::gnuplot_draw::draw_line(tmp_file.out(), segment1);
    tmp_file.out() << std::endl;

    std::clog << "Buffer:" << '\n' << buffer.str() << std::endl;

    geomtools::vector_3d shifted_pos(1.0, 0.5, 1.5);
    std::list<geomtools::polyline_3d> wires;
    std::istringstream in_wires(buffer.str());
    // Parsed polylines: p1 and p2 (index 3):
    if (plot_polylines) {
      tmp_file.out() << "# Parsed polylines:" << std::endl;
    if (geomtools::parse_wires(in_wires, wires)) {
      std::clog << "Successfull parsing of wires!" << std::endl;
      for (std::list<geomtools::polyline_3d>::const_iterator i = wires.begin();
           i != wires.end();
           i++) {
        i->tree_dump(std::clog, "Wires: ", "  ");
        std::clog << "Number of points: " << i->get_number_of_points() << std::endl;
        geomtools::gnuplot_draw::draw_polyline(tmp_file.out(),shifted_pos, identity, *i, false, false);
      }
      tmp_file.out() << std::endl;
    }
    }

    std::list<geomtools::classified_segment> csegments;
    geomtools::classify_in_out_segment(segment1, *comp1, up1, 0.05, 0.0, csegments);
    // geomtools::classify_in_out_segment(segment1, b1, up1, 0.05, 0.0, csegments);
    // geomtools::classify_in_out_segment(segment1, b2, up2, 0.05, 0.0, csegments);
    std::clog << "Number of segments: " << csegments.size() << std::endl;

    // Out segments (index 4):
    int out_count = 0;
    tmp_file.out() << "# Out segments:" << std::endl;
    for (std::list<geomtools::classified_segment>::const_iterator i = csegments.begin();
         i != csegments.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_OUTSIDE) {
        out_count++;
        geomtools::line_3d l;
        l.set_first(i->first);
        l.set_last(i->last);
        geomtools::gnuplot_draw::draw_segment(tmp_file.out(), origin, identity, l, false);
      }
    }
    if (out_count) tmp_file.out() << std::endl;

    // In segments (index 5):
    int in_count = 0;
    tmp_file.out() << "# In segments:" << std::endl;
    for (std::list<geomtools::classified_segment>::const_iterator i = csegments.begin();
         i != csegments.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_INSIDE) {
        in_count++;
        geomtools::line_3d l;
        l.set_first(i->first);
        l.set_last(i->last);
        geomtools::gnuplot_draw::draw_segment(tmp_file.out(), origin, identity, l, false);
      }
    }
    if (in_count) tmp_file.out() << std::endl;

    // Surf segments (index 6):
    int surf_count = 0;
    tmp_file.out() << "# Surf segments:" << std::endl;
    for (std::list<geomtools::classified_segment>::const_iterator i = csegments.begin();
         i != csegments.end();
         i++) {
      if (i->domain == geomtools::SHAPE_DOMAIN_ON_SURFACE) {
        surf_count++;
        geomtools::line_3d l;
        l.set_first(i->first);
        l.set_last(i->last);
        geomtools::gnuplot_draw::draw_segment(tmp_file.out(), origin, identity, l, false);
      }
    }
    if (surf_count) tmp_file.out() << std::endl;

    // Build the 'wires' rendering of the composite:
    std::list<geomtools::line_3d> composite_wires;

    geomtools::split_segment_wrt_shape SSS;
    if (!cyl2) {
      SSS.configure(b2, up2, 0.05, 0.0);
    } else {
      SSS.configure(c2, up2, 0.05, 0.0);
    }
    {
      std::ostringstream b1_draw_oss;
      geomtools::gnuplot_draw::draw_box(b1_draw_oss,
                                        up1.get_translation(),
                                        up1.get_rotation(),
                                        b1);
      std::istringstream b1_draw_iss(b1_draw_oss.str());
      std::list<geomtools::polyline_3d> b1_wires;
      geomtools::parse_wires(b1_draw_iss, b1_wires);
      SSS.add(b1_wires);
    }
    if (subtr) {
      // Subtraction:
      SSS.fetch_outside_segments(composite_wires);
      //SSS.fetch_surface_segments(composite_wires);
    } else if (inter) {
      // Intersection:
      SSS.fetch_inside_segments(composite_wires);
      SSS.fetch_surface_segments(composite_wires);
    } else {
      // Union:
      SSS.fetch_outside_segments(composite_wires);
    }
    SSS.clear();

    SSS.configure(b1, up1, 0.05, 0.0);
    {
      std::ostringstream bc2_draw_oss;
      if (!cyl2) {
        geomtools::gnuplot_draw::draw_box(bc2_draw_oss,
                                          up2.get_translation(),
                                          up2.get_rotation(),
                                          b2);
      } else {
        geomtools::gnuplot_draw::draw_cylinder(bc2_draw_oss,
                                               up2.get_translation(),
                                               up2.get_rotation(),
                                               c2);
      }
      std::istringstream bc2_draw_iss(bc2_draw_oss.str());
      std::list<geomtools::polyline_3d> bc2_wires;
      geomtools::parse_wires(bc2_draw_iss, bc2_wires);
      SSS.add(bc2_wires);
    }
    if (subtr) {
      // Subtraction:
      SSS.fetch_inside_segments(composite_wires);
      SSS.fetch_surface_segments(composite_wires);
    } else if (inter) {
      // Intersection:
      SSS.fetch_inside_segments(composite_wires);
      SSS.fetch_surface_segments(composite_wires);
    } else {
      // Union:
      SSS.fetch_outside_segments(composite_wires);
    }
    // SSS.fetch_all_segments(composite_wires);
    SSS.clear();

    // Composite boxes 1 and 2 (index 7):
    tmp_file.out() << "# Composite:" << std::endl;
    for (std::list<geomtools::line_3d>::const_iterator i = composite_wires.begin();
         i != composite_wires.end();
         i++) {
      geomtools::gnuplot_draw::draw_line(tmp_file.out(),
                                         origin, identity,
                                         *i, false);
    }
    tmp_file.out() << std::endl;

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::test_wires' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xyplane at -1.5");
      g1.cmd("set xrange [-2:+5]");
      g1.cmd("set yrange [-5:+5]");
      g1.cmd("set zrange [-2:+5]");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      {
        std::ostringstream plot_cmd;
        int index = 0;
        plot_cmd << "splot ";
        if (plot_polylines) {
          plot_cmd << "'" << tmp_file.get_filename()
                   << "' index " << index << " title 'Original' with lines ";
          index++;
        }
        if (plot_shapes) {
          plot_cmd << (index?",":"") << " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Boxes/Cylinder' with lines lt 5";
          index++;
        }
        {
          plot_cmd << (index?",":"") <<  " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Segment' with lines ";
          index++;
        }
        if (plot_polylines) {
          plot_cmd << (index?",":"") <<  " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Parsed' with lines ";
          index++;
        }
        if (out_count) {
          plot_cmd << (index?",":"")  << " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Out segments' with lines lt 1 lw 2";
          index++;
        }
        if (in_count) {
          plot_cmd << (index?",":"")  << " '" << tmp_file.get_filename();
          index++;

          plot_cmd << "' index " << index << " title 'In segments' with lines lt 2 lw 2";
        }
        if (surf_count) {
          plot_cmd << (index?",":"") << " '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Surf segments' with lines lt 3 lw 2";
          index++;
        }
        {
          plot_cmd << ", '" << tmp_file.get_filename()
                   << "' index " << index << " title 'Composite' with lines lt 4 lw 2";
          index++;
        }
      g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
#endif
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
