# TODO

## Created default opcodes in `generator.hpp`

Finished | Opcode | Type | C | Chip-8 script |
| --- | --- | --- | --- | --- | --- | --- |
| [ ] | 0NNN | Call | RCA 1802 at address NNN. | *todo* |
| [ ] | 00E0 | Displ | disp_clear()	| *todo* |
| [ ] | 00EE | Flow | return; | *todo* |	
| [ ] | 1NNN | Flow | goto NNN; | *todo* |	
| [ ] | 2NNN | Flow | *(0xNNN)() | *todo* |	
| [ ] | 3XNN | Cond | if(Vx==NN) | *todo* |	
| [ ] | 4XNN | Cond | if(Vx!=NN) | *todo* |	
| [ ] | 5XY0 | Cond | if(Vx==Vy) | *todo* |	
| [x] | 6XNN | Const | Vx = NN | <code>var x = 0</code>, <code>x = 10</code> |	
| [x] | 7XNN | Const | Vx += NN | <code>a += 8</code> |	
| [x] | 8XY0 | Assig | Vx=Vy | ~~<code>var a = b</code>~~(fails), <code>a = b</code> |
| [ ] | 8XY1 | BitOp | Vx=Vx\|Vy | *todo* |	
| [ ] | 8XY2 | BitOp | Vx=Vx&Vy | *todo* |	
| [ ] | 8XY3 | BitOp | Vx=Vx^Vy | *todo* |	
| [x] | 8XY4 | Math | Vx += Vy | <code>a += b</code> |	
| [ ] | 8XY5 | Math | Vx -= Vy | *todo* |	
| [ ] | 8XY6 | BitOp | Vx>>=1 | *todo* |	
| [ ] | 8XY7 | Math | Vx=Vy-Vx | *todo* |	
| [ ] | 8XYE | BitOp | x<<=1 | *todo* |	
| [ ] | 9XY0 | Cond | i (Vx!=Vy) | *todo* |	
| [ ] | ANNN | MEM | I = NNN | *todo* |	
| [ ] | BNNN | Flow | PC=V0+NNN | *todo* |	
| [ ] | CXNN | Rand | Vx=rand()&NN | *todo* |
| [ ] | DXYN | Disp | draw(Vx,Vy,N) | *todo* |	
| [ ] | EX9E | KeyOp | if(key()==Vx) | *todo* |	
| [ ] | EXA1 | KeyOp | if(key()!=Vx) | *todo* |	
| [ ] | FX07 | Timer | Vx = get_delay() | *todo* |	
| [ ] | FX0A | KeyOp | Vx = get_key() | *todo* |	
| [ ] | FX15 | Timer | delay_timer(Vx) | *todo* |	
| [ ] | FX18 | Sound | sound_timer(Vx) | *todo* |	
| [ ] | FX1E | MEM | I +=Vx	Adds VX | *todo* | 
| [ ] | FX29 | MEM | I=sprite_addr[Vx] | *todo* |	
| [ ] | FX33 | BCD | set_BCD(Vx); | *todo* |
| [ ] | FX55 | MEM | reg_dump(Vx,&I) | *todo* |	
| [ ] | FX65 | MEM | reg_load(Vx,&I) | *todo* |