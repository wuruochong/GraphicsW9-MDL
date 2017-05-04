/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
		     multiply the current top of the
		     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
		    temporary matrix, multiply it by the
		    current top of the origins stack, then
		    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live

  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct stack *s;
  struct matrix *edges;
  screen t;
  color g;

  s = new_stack();
  tmp = new_matrix(4, 1000);
  edges = new_matrix(4,4);
  clear_screen( t );

  for (i=0;i<lastop;i++) {
    switch (op[i].opcode) {

      case PUSH:
        printf("%d: Push\n", i);
        push(s);
        break;

      case POP:
        printf("%d: Pop\n", i);
        pop(s);
        break;

      case MOVE:
        printf("%d: Move\n", i);
        tmp = make_translate(op[i].op.move.d[0], op[i].op.move.d[1], op[i].op.move.d[2]);
        matrix_mult(peek(s),tmp);
        copy_matrix(tmp, peek(s));
        tmp->lastcol = 0;
        break;

      case SCALE:
        printf("%d: Scale\n", i);
        tmp = make_scale(op[i].op.scale.d[0], op[i].op.scale.d[1], op[i].op.scale.d[2]);
        matrix_mult(peek(s),tmp);
        copy_matrix(tmp, peek(s));
        tmp->lastcol = 0;
        break;

      case ROTATE:
        printf("%d: Rotate\n", i);
        op[i].op.rotate.degrees = op[i].op.rotate.degrees * (M_PI/180);
        if (op[i].op.rotate.axis == 0){
          tmp = make_rotX(op[i].op.rotate.degrees);
        }

        else if (op[i].op.rotate.axis == 1){
          tmp = make_rotY(op[i].op.rotate.degrees);
        }

        else if (op[i].op.rotate.axis == 2){
          tmp = make_rotZ(op[i].op.rotate.degrees);
        }
        matrix_mult(peek(s),tmp);
        copy_matrix(tmp, peek(s));
        tmp->lastcol = 0;
        break;

      case BOX:
        printf("%d: Box\n", i);
        add_box(edges, op[i].op.box.d0[0], op[i].op.box.d0[1], op[i].op.box.d0[2], op[i].op.box.d1[0], op[i].op.box.d1[1], op[i].op.box.d1[2]);
        matrix_mult(peek(s), edges);
        draw_polygons(edges, t, g);
        edges->lastcol = 0;
        break;

      case SPHERE:
        printf("%d: Sphere\n", i);
        add_sphere(edges, op[i].op.sphere.d[0], op[i].op.sphere.d[1], op[i].op.sphere.d[2], op[i].op.sphere.r, 0.05);
        matrix_mult(peek(s), edges);
        draw_polygons(edges, t, g);
        edges->lastcol = 0;
        break;

      case TORUS:
        printf("%d: Torus\n", i);
        add_torus(edges, op[i].op.torus.d[0], op[i].op.torus.d[1], op[i].op.torus.d[2],
                  op[i].op.torus.r0, op[i].op.torus.r1, 0.05);
        matrix_mult(peek(s), edges);
        draw_polygons(edges, t, g);
        edges->lastcol = 0;
        break;

      case DISPLAY:
        display(t);
        break;

      case SAVE:
        save_extension(t, op[i].op.save.p->name);
        break;

      case LINE:
        printf("%d: Line\n", i);
        add_edge(edges, op[i].op.line.p0[0], op[i].op.line.p0[1], op[i].op.line.p0[2], op[i].op.line.p1[0], op[i].op.line.p1[1], op[i].op.line.p1[2]);
        matrix_mult(peek(s), edges);
        draw_lines(edges, t, g);
        edges->lastcol = 0;

    }
    printf("\n");
  }
}
