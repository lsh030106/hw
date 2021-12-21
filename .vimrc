set nocompatible              " be iMproved, required
filetype off                  " required
" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')
" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'
" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
"Plugin 'tpope/vim-fugitive'
" plugin from http://vim-scripts.org/vim/scripts.html
" Plugin 'L9'
" Git plugin not hosted on GitHub
"Plugin 'git://git.wincent.com/command-t.git'
" git repos on your local machine (i.e. when working on your own plugin),
"Plugin 'file:///home/jjeaby/Dev/tools/vim-plugin'
Plugin 'vim-airline/vim-airline'
Plugin 'scrooloose/nerdtree'
"Plugin 'airblade/vim-gitgutter'
Plugin 'scrooloose/syntastic'
"Plugin 'ctrlpvim/ctrlp.vim'
Plugin 'nanotech/jellybeans.vim'
Plugin 'Valloric/YouCompleteMe', { 'commit' : 'd98f896' }
call vundle#end()            " required
"filetype plugin indent on    " required
"NERDTree ON 단축키를 "\nt"로 설정
map <Leader>nt <ESC>:NERDTree<CR>
let NERDTreeShowHidden=1
" let NERDTreeQuitOnOpen=1
color jellybeans

" Tag List 환경설정
filetype on                                 "vim filetype on
"Source Explorer 환경설정
nmap <F8> :SrcExplToggle<CR>                "F8 Key = SrcExpl Toggling
nmap <C-H> <C-W>h                           "왼쪽 창으로 이동
nmap <C-J> <C-W>j                           "아래 창으로 이동
nmap <C-K> <C-W>k                           "윗 창으로 이동
nmap <C-L> <C-W>l                           "오른쪽 창으로 이동
" 세부 정보 출력
set nu
set title
set showmatch
set ruler

" 구문 강조 사용

if has("sybtax")
	syntax on
endif

" 색깔 설정
set t_Co=256
" 들여쓰기 설정

set autoindent
set cindent
set smartindent

set tabstop=4
set shiftwidth=4
set softtabstop=4
set smarttab
set expandtab
" 붙여넣기 설정
"set paste 이거 설정하면 들여쓰기 안됨
set mouse-=a	
" 한글 입력 설정
set encoding=utf-8
set termencoding=utf-8
" 커서가 있는 줄을 강조함
set cursorline
" 상태바 표시를 항상한다
set laststatus=2
set statusline=\ %<%l:%v\ [%P]%=%a\ %h%m%r\ %F\
" 검색 설정
set ignorecase
" 마지막으로 수정된 곳에 커서를 위치함
au BufReadPost *
\ if line("'\"") > 0 && line("'\"") <= line("$") |
\ exe "norm g`\"" |
\ endif

" Markdown 문법 설정 (Git 에서 사용)
augroup markdown
    " remove previous autocmds
    autocmd!
    " set every new or read *.md buffer to use the markdown filetype
    autocmd BufRead,BufNew *.md setf markdown
augroup END

let g:ycm_global_ycm_extra_conf = '~/.vim/.ycm_extra_conf.py'

let g:ycm_confirm_extra_conf = 0

"To avoid conflict snippets

let g:ycm_key_list_select_completion = ['<C-j>', '<Down>']

let g:ycm_key_list_previous_completion = ['<C-k>', '<Up>']

let g:ycm_autoclose_preview_window_after_completion = 1

nnoremap <leader>g :YcmCompleter GoTo<CR>

nnoremap <leader>gg :YcmCompleter GoToImprecise<CR>

nnoremap <leader>d :YcmCompleter GoToDeclaration<CR>

nnoremap <leader>t :YcmCompleter GetType<CR>

nnoremap <leader>p :YcmCompleter GetParent<CR>
