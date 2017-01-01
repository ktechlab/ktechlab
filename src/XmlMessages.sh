function get_files
{
    echo ktechlab_mime.xml
}

function po_for_file
{
    case "$1" in
       ktechlab_mime.xml)
           echo ktechlab_xml_mimetypes.po
       ;;
    esac
}

function tags_for_file
{
    case "$1" in
       ktechlab_mime.xml)
           echo comment
       ;;
    esac
}
