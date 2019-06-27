/**
  Copyright © 2017 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#include "sm3.h"
#include <stdio.h>

#define F1(x,y,z)(((x)^(y)^(z)))
#define FF(x,y,z)(((x)&(y))^((x)&(z))^((y)&(z))) 
#define GG(x,y,z)(((x)&(y))^(~(x)&(z)))

#define P0(x)x^R(x,9)^R(x,17)
#define P1(x)x^R(x,15)^R(x,23)


unsigned int bswap32(unsigned int x){
  unsigned int result;
  unsigned char* out = &result;
  unsigned char* input = &x;
  out[3] = input[0];
  out[2] = input[1];
  out[1] = input[2];
  out[0] = input[3];
  return result;
}

unsigned long bswap64(unsigned long x){
  unsigned long result;
  unsigned char* out = &result;
  unsigned char* input = &x;

  out[7] = input[0];
  out[6] = input[1];
  out[5] = input[2];
  out[4] = input[3];
  out[3] = input[4];
  out[2] = input[5];
  out[1] = input[6];
  out[0] = input[7];
  return result;
}


// W R_replace(W v,int n){

//   W r0 = (v) << (n);
//   W r1 = (v) >> (32-n);
//   W r = r0 | r1;  
//   printf("v %u, n %d\n", v,n);
//   printf("r0 %u, r1 %u, r %u\n", r0, r1, r);
//   return r;
// }

void sm3_compress(sm3_ctx *c) {
    W t1,t2,i,t,s1,s2,x[8],w[68];

    // load data
    F(16)w[i]=rev32(c->x.w[i]);
    for(i=16;i<68;i++)
      w[i]=P1(w[i-16]^w[i-9]^R(w[i-3],15))^R(w[i-13],7)^w[i- 6];

    F(8)x[i]=c->s[i];
    // compress data
    F(64) {
      t=(i<16)?0x79cc4519:0x7a879d8a;
      s2=R(x[0],12);      
      s1=R(s2+x[4]+R(t,i),7);

      s2^=s1;
      if(i<16) {
        t1=F1(x[0],x[1],x[2])+x[3]+s2+(w[i]^w[i+4]);
        t2=F1(x[4],x[5],x[6])+x[7]+s1+w[i];
      } else {
        t1=FF(x[0],x[1],x[2])+x[3]+s2+(w[i]^w[i+4]);
        t2=GG(x[4],x[5],x[6])+x[7]+s1+w[i];      
      }
      x[3]=x[2];x[2]=R(x[1],9);x[1]=x[0];x[0]=t1;
      x[7]=x[6];x[6]=R(x[5],19);x[5]=x[4];x[4]=P0(t2);     
    }
    F(8)c->s[i]^=x[i];
}

void sm3_init(sm3_ctx *c) {    
    c->s[0]=0x7380166f;
    c->s[1]=0x4914b2b9;
    c->s[2]=0x172442d7;
    c->s[3]=0xda8a0600;
    c->s[4]=0xa96f30bc;
    c->s[5]=0x163138aa;
    c->s[6]=0xe38dee4d;
    c->s[7]=0xb0fb0e4e;
    c->len =0;
}

void sm3_update(sm3_ctx *c, const void *in, W len) {
    B *p=(B*)in;
    W i, idx;
    
    // index = len % 64
    idx = c->len & 63;
    // update total length
    c->len += len;
    
    for (i=0;i<len;i++) {
      // add byte to buffer
      c->x.b[idx]=p[i]; idx++;
      // buffer filled?
      if(idx==64) {
        // compress it

        sm3_compress(c);
        idx=0;
      }
    }
}

void sm3_final(void *out, sm3_ctx *c) {
    W i,len;
    W *p=(W*)out;
    
    // get index
    i = len = c->len & 63;
    // zero remainder of buffer
    while(i < 64) c->x.b[i++]=0;
    // add 1 bit
    c->x.b[len]=0x80;
    
    // exceeds or equals area for total bits?
    if(len >= 56) {
      // compress it
      sm3_compress(c);
      // zero buffer
      F(16)c->x.w[i]=0;
    }
    // add total length in bits
    c->x.q[7]=rev64(c->len*8);
    
    // compress it
    sm3_compress(c);
    // return hash
    F(8)p[i]=rev32(c->s[i]);
}

