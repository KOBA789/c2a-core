#!/bin/bash
echo setup.sh
echo setting for local environment...
echo
echo [SET USER HOOKS]
cp -f ./Script/Git/pre-commit.pl ./.git/hooks/pre-commit
chmod 755 ./.git/hooks/pre-commit
echo done.
echo
echo [SET UP DEVELOP ENVIRONMENT FOR C2A MINIMUM USER SAMPLE]
if [ -e './Examples/minimum_user_for_s2e/src/src_core' ]; then
  rm -rf './Examples/minimum_user_for_s2e/src/src_core'
fi
ln -s '../../../' './Examples/minimum_user_for_s2e/src/src_core'
echo done.
echo
echo The process has been completed.