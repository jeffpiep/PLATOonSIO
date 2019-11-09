/*
 * SIO2Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SIO2Arduino; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

//#import "sdrive.h"
//#import "config.h"
#include "sdrive.h" // fix to standard C++ directive
#include "config.h" // fix to standard C++ directive

SDriveHandler::SDriveHandler()
{
}

void SDriveHandler::setDriveControl(DriveControl *driveControl)
{
  m_driveControl = driveControl;
}

boolean SDriveHandler::isValidCommand(byte cmd)
{
  return (cmd == CMD_SDRIVE_IDENT ||
          cmd == CMD_SDRIVE_INIT ||
          cmd == CMD_SDRIVE_CHROOT ||
          cmd == CMD_SDRIVE_SWAP_VDN ||
          cmd == CMD_SDRIVE_GETPARAMS ||
          cmd == CMD_SDRIVE_GET_ENTRIES ||
          cmd == CMD_SDRIVE_CHDIR_VDN ||
          cmd == CMD_SDRIVE_CHDIR ||
          cmd == CMD_SDRIVE_CHDIR_UP ||
          cmd == CMD_SDRIVE_GET20 ||
          cmd == CMD_SDRIVE_MOUNT_D0 ||
          cmd == CMD_SDRIVE_MOUNT_D1 ||
          cmd == CMD_SDRIVE_MOUNT_D2 ||
          cmd == CMD_SDRIVE_MOUNT_D3 ||
          cmd == CMD_SDRIVE_MOUNT_D4);
}

boolean SDriveHandler::isValidDevice(byte device)
{
  return (device == DEVICE_SDRIVE);
}

void SDriveHandler::processCommand(CommandFrame *cmdFrame, Stream *stream)
{
  #ifdef DEBUG1
  LOGGING_UART.println("SDrive Processing Command");
  LOGGING_UART.flush();
#endif
  switch (cmdFrame->command)
  {
  case CMD_SDRIVE_IDENT:
    cmdIdent(stream);
    break;
  case CMD_SDRIVE_INIT:
    cmdInit(stream);
    break;
  case CMD_SDRIVE_CHROOT:
    cmdChroot(stream);
    break;
  case CMD_SDRIVE_SWAP_VDN:
    cmdSwapVdn(stream);
    break;
  case CMD_SDRIVE_GETPARAMS:
    cmdGetParams(stream);
    break;
  case CMD_SDRIVE_GET_ENTRIES:
    cmdGetEntries(cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_CHDIR_VDN:
    cmdChdirVDN(stream);
    break;
  case CMD_SDRIVE_CHDIR_UP:
    cmdChdirUp((cmdFrame->aux1 > 0), stream);
    break;
  case CMD_SDRIVE_CHDIR:
    cmdChdir(cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_GET20:
    cmdGet20(cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_MOUNT_D0:
    cmdMountDrive(0, cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_MOUNT_D1:
    cmdMountDrive(1, cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_MOUNT_D2:
    cmdMountDrive(2, cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_MOUNT_D3:
    cmdMountDrive(3, cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  case CMD_SDRIVE_MOUNT_D4:
    cmdMountDrive(4, cmdFrame->aux2 * 256 + cmdFrame->aux1, stream);
    break;
  }
}

void SDriveHandler::cmdIdent(Stream *stream)
{
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  stream->write("SDrive01");
  stream->write(0xB0);
  stream->flush();
}

void SDriveHandler::cmdInit(Stream *stream)
{
  // NO-OP
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  stream->flush();
}

void SDriveHandler::cmdChroot(Stream *stream)
{
  // NO-OP
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  stream->flush();
}

void SDriveHandler::cmdSwapVdn(Stream *stream)
{
  // NO-OP
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  stream->flush();
}

void SDriveHandler::cmdGetParams(Stream *stream)
{
  // NO-OP
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  stream->write(0x06);
  stream->write((byte)0x00);
  stream->write(0x06);
  stream->flush();
}

void SDriveHandler::cmdGetEntries(byte n, Stream *stream)
{
  // NO-OP
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  for (int i = 0; i < n * 12; i++)
  {
    stream->write((byte)0x00);
  }
  stream->write((byte)0x00);
  stream->flush();
}

void SDriveHandler::cmdChdirVDN(Stream *stream)
{
  // NO-OP
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  for (int i = 0; i < 14; i++)
  {
    stream->write((byte)0x00);
  }
  stream->write((byte)0x00);
  stream->flush();
}

void SDriveHandler::cmdChdirUp(bool getDirName, Stream *stream)
{
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);

  m_driveControl->changeDir(-1);

  stream->write(COMPLETE);

  if (getDirName)
  {
    stream->write('F');
    for (int i = 0; i < 10; i++)
    {
      stream->write(' ');
    }
    stream->write((byte)19);
    stream->write((byte)0);
    stream->write((byte)0);
  }

  stream->flush();
}

void SDriveHandler::cmdChdir(int ix, Stream *stream)
{
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);

  m_driveControl->changeDir(ix);

  stream->write(COMPLETE);
  stream->flush();
}

void SDriveHandler::cmdGet20(int page, Stream *stream)
{
  delay(DELAY_T2);
  stream->write(ACK);
  delay(DELAY_T5);
  stream->write(COMPLETE);
  byte chkSum = 0;

  FileEntry fileEntries[21];
  for (int i = 0; i < 21; i++)
  {
    memset(fileEntries[i].name, 0, 11);
    fileEntries[i].isDirectory = false;
  }

  int count = m_driveControl->getFileList(page, 21, fileEntries);

  char c;
  for (int i1 = 0; i1 < 20; i1++)
  {
    FileEntry entry = fileEntries[i1];
    for (int i2 = 0; i2 < 11; i2++)
    {
      c = entry.name[i2];
      stream->write(c);
      chkSum = ((chkSum + c) >> 8) + ((chkSum + c) & 0xff);
    }
    if (entry.isDirectory)
    {
      stream->write((byte)19);
      chkSum = ((chkSum + 19) >> 8) + ((chkSum + 19) & 0xff);
    }
    else
    {
      stream->write((byte)0x00);
    }
  }

  if (count > 20)
  {
    stream->write(fileEntries[20].name[0]);
  }
  else
  {
    stream->write((byte)0x00);
  }

  stream->write(chkSum);
  stream->flush();
}

void SDriveHandler::cmdMountDrive(byte driveNum, byte index, Stream *stream)
{
  delay(DELAY_T2);
  stream->write(ACK);

  m_driveControl->mountFile(1, index);

  delay(DELAY_T5);
  stream->write(COMPLETE);
  stream->flush();
}

boolean SDriveHandler::printCmdName(byte cmd)
{
// we only compile this on DEBUG to save allocating string constants
#ifdef DEBUG1
  switch (cmd)
  {
  case CMD_SDRIVE_IDENT:
    LOGGING_UART.print("SDRIVE IDENT");
    break;
  case CMD_SDRIVE_INIT:
    LOGGING_UART.print("SDRIVE INIT");
    break;
  case CMD_SDRIVE_CHROOT:
    LOGGING_UART.print("SDRIVE CHROOT");
    break;
  case CMD_SDRIVE_SWAP_VDN:
    LOGGING_UART.print("SDRIVE SWAP VDN");
    break;
  case CMD_SDRIVE_GETPARAMS:
    LOGGING_UART.print("SDRIVE GETPARAMS");
    break;
  case CMD_SDRIVE_GET_ENTRIES:
    LOGGING_UART.print("SDRIVE GET ENTRIES");
    break;
  case CMD_SDRIVE_CHDIR_VDN:
    LOGGING_UART.print("SDRIVE CHDIR VDN");
    break;
  case CMD_SDRIVE_CHDIR:
    LOGGING_UART.print("SDRIVE CHDIR");
    break;
  case CMD_SDRIVE_CHDIR_UP:
    LOGGING_UART.print("SDRIVE CHDIR UP");
    break;
  case CMD_SDRIVE_GET20:
    LOGGING_UART.print("SDRIVE GET20");
    break;
  case CMD_SDRIVE_MOUNT_D0:
    LOGGING_UART.print("SDRIVE MOUNTvD0");
    break;
  case CMD_SDRIVE_MOUNT_D1:
    LOGGING_UART.print("SDRIVE MOUNTvD1");
    break;
  case CMD_SDRIVE_MOUNT_D2:
    LOGGING_UART.print("SDRIVE MOUNTvD2");
    break;
  case CMD_SDRIVE_MOUNT_D3:
    LOGGING_UART.print("SDRIVE MOUNTvD3");
    break;
  case CMD_SDRIVE_MOUNT_D4:
    LOGGING_UART.print("SDRIVE MOUNTvD4");
    break;
  default:
    return false;
  }
  LOGGING_UART.flush();
#endif

  return true;
}
