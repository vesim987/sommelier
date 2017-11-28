# Not bash or zsh?
[ -n "$BASH_VERSION" -o -n "$ZSH_VERSION" ] || return 0

# DISPLAY already set?
[ -z "$DISPLAY" ] || return 0

# Not running under xwl?
[ $(systemctl --user show-environment | grep ^XWL_VERSION=) ] || return 0

export $(systemctl --user show-environment | grep ^DISPLAY=)
export _JAVA_AWT_WM_NONREPARENTING=1
