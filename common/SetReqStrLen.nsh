!macro SetReqStrLen Req_STRLEN
!define "Check_${NSIS_MAX_STRLEN}"
!ifndef "Check_${Req_STRLEN}"
 !error "You're not using the ${Req_STRLEN} string length special build! \
 ${NSIS_MAX_STRLEN} is no good!"
!else
 !undef "Check_${NSIS_MAX_STRLEN}"
 !undef "SetReqStrLen"
!endif
!macroend
!define SetReqStrLen "!insertmacro SetReqStrLen"
