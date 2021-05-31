

section .data

msg db "Hola", 0x0A, 0

section .text
	global main
	extern printf

prn:
	;;push ebp
        ;;mov ebp, esp
	;;sub esp, 4
	;;mov [esp], eax
	;;mov eax, msg
	;;push eax ;; -4 resta 4 sub esp, mov [esp], eax	
	push msg
	call printf
	add esp, 4 ;; para dejarlo como está poner esp donde estaba

	mov esp, ebp
	pop ebp 
	ret

main:
	push ebp
	mov ebp, esp

	call prn
	xor eax, eax

	mov esp, ebp
	pop ebp
	ret
