; This program asks user for a name (by displaying a question) and the prints it
; User input is terminated with Tab key
:L 0
; tab key
:B 9
:G *stringstart
:C *stringstart
;
; printing/asking loop
;
s+ C -L> L
*command p L
s+ L -W> C
++ C
if L != B
end stack_jump
s+ G -R> C
jump 0
;
; main program loop
;
; ask command
s+ 2 -C> *command
end stack_jump
; print command
s+ 4 -C> *command
end stack_jump
end normal
*stringstart . aW
. ah
. aa
. at
. a'
. as
. 32
. ay
. ao
. au
. ar
. 32
. an
. aa
. am
. ae
. a?
. 9
