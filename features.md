# Features

## List of all chip8 opcodes and their chip8-script equivalent
Under construction :construction:

| Supported | Opcode | Type | C | Chip-8 script |
| --- | --- | --- | --- | --- |
| <ul><li>- [ ] </li></ul> | 0NNN | Call | RCA 1802 at address NNN. | *todo* |
| <ul><li>- [ ] </li></ul> | 00E0 | Displ | disp_clear()	| *todo* |
| <ul><li>- [ ] </li></ul> | 00EE | Flow | return; | *todo* |	
| <ul><li>- [ ] </li></ul> | 1NNN | Flow | goto NNN; | *todo* |	
| <ul><li>- [ ] </li></ul> | 2NNN | Flow | *(0xNNN)() | *todo* |	
| <ul><li>- [ ] </li></ul> | 3XNN | Cond | if(Vx==NN) | *todo* |	
| <ul><li>- [ ] </li></ul> | 4XNN | Cond | if(Vx!=NN) | *todo* |	
| <ul><li>- [ ] </li></ul> | 5XY0 | Cond | if(Vx==Vy) | *todo* |	
| <ul><li>- [x] </li></ul> | 6XNN | Const | Vx = NN | <code>var x = 0</code>, <code>x = 10</code> |	
| <ul><li>- [x] </li></ul> | 7XNN | Const | Vx += NN | <code>a += 8</code> |	
| <ul><li>- [x] </li></ul> | 8XY0 | Assig | Vx=Vy | <code>var a = b</code>, <code>a = b</code> |
| <ul><li>- [x] </li></ul> | 8XY1 | BitOp | Vx=Vx\|Vy | <code>a \|= b</code> |	
| <ul><li>- [x] </li></ul> | 8XY2 | BitOp | Vx=Vx&Vy | <code>a &= b</code> |	
| <ul><li>- [x] </li></ul> | 8XY3 | BitOp | Vx=Vx^Vy | <code>a ^= b</code> |	
| <ul><li>- [x] </li></ul> | 8XY4 | Math | Vx += Vy | <code>a += b</code> |	
| <ul><li>- [x] </li></ul> | 8XY5 | Math | Vx -= Vy | <code>a -= b</code> |	
| <ul><li>- [ ] </li></ul> | 8XY6 | BitOp | Vx>>=1 | *todo* |	
| <ul><li>- [ ] </li></ul> | 8XY7 | Math | Vx=Vy-Vx | *todo* |	
| <ul><li>- [ ] </li></ul> | 8XYE | BitOp | x<<=1 | *todo* |	
| <ul><li>- [ ] </li></ul> | 9XY0 | Cond | i (Vx!=Vy) | *todo* |	
| <ul><li>- [ ] </li></ul> | ANNN | MEM | I = NNN | *todo* |	
| <ul><li>- [ ] </li></ul> | BNNN | Flow | PC=V0+NNN | *todo* |	
| <ul><li>- [ ] </li></ul> | CXNN | Rand | Vx=rand()&NN | *todo* |
| <ul><li>- [ ] </li></ul> | DXYN | Disp | draw(Vx,Vy,N) | *todo* |	
| <ul><li>- [ ] </li></ul> | EX9E | KeyOp | if(key()==Vx) | *todo* |	
| <ul><li>- [ ] </li></ul> | EXA1 | KeyOp | if(key()!=Vx) | *todo* |	
| <ul><li>- [ ] </li></ul> | FX07 | Timer | Vx = get_delay() | *todo* |	
| <ul><li>- [ ] </li></ul> | FX0A | KeyOp | Vx = get_key() | *todo* |	
| <ul><li>- [ ] </li></ul> | FX15 | Timer | delay_timer(Vx) | *todo* |	
| <ul><li>- [ ] </li></ul> | FX18 | Sound | sound_timer(Vx) | *todo* |	
| <ul><li>- [ ] </li></ul> | FX1E | MEM | I +=Vx	Adds VX | *todo* | 
| <ul><li>- [ ] </li></ul> | FX29 | MEM | I=sprite_addr[Vx] | *todo* |	
| <ul><li>- [ ] </li></ul> | FX33 | BCD | set_BCD(Vx); | *todo* |
| <ul><li>- [ ] </li></ul> | FX55 | MEM | reg_dump(Vx,&I) | *todo* |	
| <ul><li>- [ ] </li></ul> | FX65 | MEM | reg_load(Vx,&I) | *todo* |
