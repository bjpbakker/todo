#!/bin/sh

git_tagname_of_head() {
    local name=''
    name=$(git describe HEAD)
    if [ $? -eq 0 ]; then
        echo $name
    fi
}

git_sha_of_head() {
    sha1=$(git log -1 --pretty='%H' HEAD)
    echo $sha1
}

name=$(git_tagname_of_head)
is_release=''
ref=$(git_sha_of_head)
if [ -z "$name" ]; then
    is_release='0'
else
    is_release='1'
fi

cat <<EOF
#ifndef VERSION_H
#define VERSION_H

#define VERSION_IS_RELEASE ${is_release}
#define VERSION_GIT_TAG "${name}"
#define VERSION_GIT_REF "${ref}"

#endif
EOF

