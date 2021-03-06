#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
 // TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();
  rtl_pop(&id_dest->val);
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_lr(&id_dest->val,R_EBP,4);
  rtl_sr(R_ESP,4,&id_dest->val);
  rtl_pop(&id_dest->val);
  rtl_sr(R_EBP,4,&id_dest->val);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    rtl_lr_w(&t0,R_AX);
    rtl_sext(&t0,&t0,2);
    rtl_sari(&t0,&t0,16);
    rtl_sr_w(R_DX,&t0);
  }
  else {
    //TODO();
    rtl_lr_l(&t0,R_EAX);
    rtl_sari(&t0,&t0,31);
    rtl_sari(&t0,&t0,1);
    rtl_sr_l(R_EDX,&t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
   // TODO();
   rtl_lr_b(&t0,R_AL);
   rtl_sext(&t0,&t0,1);
   rtl_sr_w(R_AX,&t0);
  }
  else {
   // TODO();
   rtl_lr_w(&t0,R_AX);
   rtl_sext(&t0,&t0,2);
   rtl_sr_l(R_EAX,&t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
