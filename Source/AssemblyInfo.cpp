/* =========================================================================
    AssemblyInfo.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/04 0:39:16 R-Koubou
               1st Edition.

   ======================================================================== */

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// �A�Z���u���Ɋւ����ʏ��͈ȉ��̑����Z�b�g���Ƃ����Đ��䂳��܂��B
// �A�Z���u���Ɋ֘A�t�����Ă������ύX����ɂ́A
// �����̑����l��ύX���Ă��������B
//
[ assembly: AssemblyTitleAttribute( "Title" ) ];
[ assembly: AssemblyDescriptionAttribute( "" ) ];
[ assembly: AssemblyConfigurationAttribute( "" ) ];
[ assembly: AssemblyCompanyAttribute( "" ) ];
[ assembly: AssemblyProductAttribute( "Product Name" ) ];
[ assembly: AssemblyCopyrightAttribute( "Copyright (c)  2013 R-Koubou" ) ];
[ assembly: AssemblyTrademarkAttribute( "" ) ];
[ assembly: AssemblyCultureAttribute( "" ) ];

//
// �A�Z���u���̃o�[�W�������́A�ȉ��� 4 �̒l�ō\������Ă��܂�:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// ���ׂĂ̒l���w�肷�邩�A���̂悤�� '*' ���g���ă��r�W��������уr���h�ԍ���
// ����l�ɂ��邱�Ƃ��ł��܂�:

[ assembly: AssemblyVersionAttribute( "1.0.*" ) ];
[ assembly: ComVisible( false ) ];
[ assembly: CLSCompliantAttribute( true ) ];
[ assembly: SecurityPermission( SecurityAction::RequestMinimum, UnmanagedCode = true ) ];
