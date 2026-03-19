set tags=./tags,tags

let g:gutentags_ctags_extra_args = [
	\ '--c-kinds=+tfvse',
	\ '--languages=C',
	\ '--langmap=C:.c.h',
	\ '--exclude=*.toml'
	\ ]
let g:gutentags_file_list_command = 'find . /opt/homebrew/include/SDL3/ \( -name "*.c" -o -name "*.h" \)'

function! LoadCTypedefs()
	let l:tagsfile = findfile('tags', expand('%:p:h') . ';' . getcwd())
	if l:tagsfile == ''
		return
	endif

	let l:types = []
	for l:line in readfile(l:tagsfile)
		if l:line[0] == '!'
			continue
		endif
		let l:fields = split(l:line, '\t')
		if len(l:fields) >= 4 && l:fields[3] == 't'
			call add(l:types, l:fields[0])
		endif
	endfor

	if !empty(l:types)
		execute 'syntax keyword cType ' . join(l:types)
	endif
endfunction

augroup c_typedef_highlight
	autocmd!
	autocmd FileType,BufEnter,BufRead *.c,*.h call LoadCTypedefs()
augroup END
