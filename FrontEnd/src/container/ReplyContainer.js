import Reply from '../component/card/Reply.js';
import { connect } from 'react-redux';

const mapStateToProps = state => ({
  obiews: state.obiews,
})

export default connect(mapStateToProps)(Reply)
