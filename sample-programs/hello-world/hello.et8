; Hello World program using string in program space
:L a.
:C *stringstart
s+ C -B> *print
; print,1 does not work because first byte is operation not argument, mli notation
; just presents it like that
*print,2 s+ 0 -M> B
if L == B
end normal
p B
++ C
end reset
*stringstart . aH
. ae
. al
. al
. ao
. 32
. aw
. ao
. ar
. al
. ad
. a!
. a.
